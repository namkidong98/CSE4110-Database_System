#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "mysql.h"

#define MAXLINE 8192

#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
const char* pw = "rlakf3emd";
const char* db = "db3";

int main(void) {
	MYSQL* connection = NULL;
	MYSQL conn;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;

	FILE* fp;
	char line[MAXLINE], query[MAXLINE];
	char filename[100];
	int command, sub_command, truck, year, month;
	int state = 0;

	fp = fopen("20180032.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "No Open Error\n");
		exit(0);
	}

	if (mysql_init(&conn) == NULL)
		printf("mysql_init() error!");

	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	else
	{
		//printf("Connection Succeed\n");
		if (mysql_select_db(&conn, db))
		{
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 1;
		}

		while (fgets(line, MAXLINE, fp) == line) { //txt파일이 끝날 때까지 문장을 읽어와서
			mysql_query(connection, line); //MYSQL 데이터베이스에 쿼리를 전송한다
		}
		fclose(fp); //txt파일에서 읽는 작업이 끝났으면 미리 파일을 닫는다

		while (1) {
			printf("\n------- SELECT QUERY TYPES -------\n");
			printf("\t1. TYPE I\n");
			printf("\t2. TYPE II\n");
			printf("\t3. TYPE III\n");
			printf("\t4. TYPE IV\n");
			printf("\t5. TYPE V\n");
			printf("\t0. QUIT\n");
			printf("----------------------------------\n");
			printf("COMMAND: ");
			scanf("%d", &command);

			if (command == 0) return 0;

			switch (command) {
				case 1:
					while (1) {
						printf("\nInput truck number:  ");
						scanf("%d", &truck);
						if (truck == 1721 || truck == 0) break;
						else printf("Truck %d is not destroyed.(Put 0 if you wanna go back to MENU)\n\n", truck);
					}
					if (truck == 1721) {

						while (1) {
							int back = 0;
							printf("\n----- SUBTYPES IN TYPE I -----\n");
							printf("\t1. TYPE I-1.\n");
							printf("\t2. TYPE I-2.\n");
							printf("\t3. TYPE I-3.\n");
							printf("\t0. GO BACK TO MENU\n");
							printf("------------------------------\n");
							printf("COMMAND: ");
							scanf("%d", &sub_command);

							switch (sub_command) {
								case 0:
									back = 1;
									break;
								case 1:
									printf("\n---------- TYPE I-1 ----------\n");
									printf("** Find all customers who had a package on the truck at the time of the crash **\n");
									strcpy(query, "With A(package_ID) as (select package_ID From delivery_info Where vehicle_ID = '1721' and delivery_status = 'crash'), B(customer_ID) as (Select distinct customer_ID From package, A Where package.package_ID = A.package_ID) Select customer.customer_ID, customer.name, customer.birth, customer.phone_num From customer, B Where customer.customer_ID = B.customer_ID");
									state = mysql_query(connection, query);
									if (state == 0) {
										sql_result = mysql_store_result(connection);
										printf("Customer_ID     Name       Birth           Phone Number\n");
										while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
										{
											printf("%-15s %-10s %-15s %-15s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
										}
										mysql_free_result(sql_result);
									}
									else { //QUERY가 제대로 작동하지 않은 상태에 대한 예외 처리
										fprintf(stderr, "QUERY ERROR\n");
										exit(0);
									}
									printf("------------------------------\n");
									break;
								case 2:
									printf("\n---------- TYPE I-2 ----------\n");
									printf("** Find all recipients who had a package on that truck at the time of the crash **\n");
									strcpy(query, "Select distinct recipient_name, recipient_address, recipient_phone_num From package, (select package_ID From delivery_info Where vehicle_ID = '1721' and delivery_status = 'crash') as A Where package.package_ID = A.package_ID");
									state = mysql_query(connection, query);
									if (state == 0) {
										sql_result = mysql_store_result(connection);
										printf("Name       Address         Phone Number\n");
										while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
										{
											printf("%-10s %-15s %-15s\n", sql_row[0], sql_row[1], sql_row[2]);
										}
										mysql_free_result(sql_result);
									}
									else { //QUERY가 제대로 작동하지 않은 상태에 대한 예외 처리
										fprintf(stderr, "QUERY ERROR\n");
										exit(0);
									}
									printf("------------------------------\n");
									break;
								case 3:
									printf("\n---------- TYPE I-3 ----------\n");
									printf("** Find the last successful delivery by that truck prior to the crash **\n");
									strcpy(query, "select A.package_ID from delivery_info as A, (select max(cur_time) as cur_time from Delivery_Info where vehicle_ID = '1721' and delivery_status = 'done') as B where B.cur_time = A.cur_time and vehicle_ID = '1721' and delivery_status = 'done'");
									state = mysql_query(connection, query);
									if (state == 0) {
										sql_result = mysql_store_result(connection);
										while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
										{
											printf("%s\n", sql_row[0]);
										}
										mysql_free_result(sql_result);
									}
									else { //QUERY가 제대로 작동하지 않은 상태에 대한 예외 처리
										fprintf(stderr, "QUERY ERROR\n");
										exit(0);
									}
									printf("------------------------------\n");
									break;
								default:
									printf("Wrong Number!\n");
									continue; //잘못된 입력이 들어오면 다시 입력을 받는다
							}
							if (back) break;

						}

					}
					break; //switch문을 나가 다시 while loop을 돌아서 초기 메뉴로 이동
					
				case 2:
					printf("\n---------- TYPE II ----------\n");
					printf("** Find the customer who has shipped the most packages in certain year **\n");
					printf("(If you wanna go back to MENU, Put 0 as input)\n");
					while (1) {
						printf("\nWhich Year? : ");
						scanf("%d", &year);
						if (year == 0) break; //0이 입력일 때만 while loop을 빠져나가도록
						sprintf(query, "with Bill_year(c_ID, p_ID) as (select customer_ID, package_ID from Bill where due_date like '%%%d%%'), A(c_ID, cnt) as (select c_ID, count(p_ID) from Bill_year group by c_ID) select distinct customer.customer_ID, customer.name, customer.birth, customer.phone_num, B.cnt from customer, (select c_ID, cnt from A where cnt = (select max(cnt) from A)) as B where B.c_ID = customer_ID", year);
						state = mysql_query(connection, query); //에러가 발생하면 state에 0이 아닌 값이 할당
						if (state == 0) { 

							sql_result = mysql_store_result(connection);
							if ((sql_row = mysql_fetch_row(sql_result)) == NULL) { //Query에 해당하는 결과가 없을 때
								printf("No data for %d year\n", year);
								mysql_free_result(sql_result);
								continue; //다른 year를 받기 위해 while loop을 다시 돈다
							}

							else { //Query에 해당하는 결과가 있을 때
								mysql_query(connection, query);
								sql_result = mysql_store_result(connection);
								printf("\nCustomer_ID     Name       Birth           Phone Number    Num of Packages\n");
								while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
								{
									int count = atoi(sql_row[4]);
									printf("%-15s %-10s %-15s %-15s %-10d\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], count);
								}
								mysql_free_result(sql_result);
								continue; //출력을 완료 했으면 다시 year을 받기 위해 while loop을 돈다
							}
							
						}
						else { //QUERY가 제대로 작동하지 않은 상태에 대한 예외 처리
							fprintf(stderr, "QUERY ERROR\n");
							exit(0);
						}

					}
					printf("-----------------------------\n");
					break; //switch문을 나가 다시 while loop을 돌아서 초기 메뉴로 이동 

				case 3:
					printf("\n---------- TYPE III ----------\n");
					printf("** Find the customer who has shipped the most money in certain year **\n");
					printf("(If you wanna go back to MENU, Put 0 as input)\n");
					while (1) {
						printf("\nWhich Year? : ");
						scanf("%d", &year);
						if (year == 0) break; //0이 입력일 때만 while loop을 빠져나가도록
						sprintf(query, "with Bill_year(c_ID, charge) as (select customer_ID, charge from Bill where due_date like '%%%d%%'), A(c_ID, sum) as (select c_ID, sum(charge) from Bill_year group by c_ID) select distinct customer.customer_ID, customer.name, customer.birth, customer.phone_num, B.sum from customer, (select c_ID, sum from A where sum = (select max(sum) from A)) as B where B.c_ID = customer_ID", year);
						state = mysql_query(connection, query); //에러가 발생하면 state에 0이 아닌 값이 할당
						if (state == 0) {

							sql_result = mysql_store_result(connection);
							if ((sql_row = mysql_fetch_row(sql_result)) == NULL) { //Query에 해당하는 결과가 없을 때
								printf("No data for %d year\n", year);
								mysql_free_result(sql_result);
								continue; //다른 year를 받기 위해 while loop을 다시 돈다
							}

							else { //Query에 해당하는 결과가 있을 때
								mysql_query(connection, query);
								sql_result = mysql_store_result(connection);
								printf("\nCustomer_ID     Name       Birth           Phone Number    Money Spent\n");
								while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
								{
									float sum = atof(sql_row[4]);
									printf("%-15s %-10s %-15s %-15s %-10.3f\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sum);
								}
								mysql_free_result(sql_result);
								continue; //출력을 완료 했으면 while loop을 빠져나가도록
							}

						}
						else { //QUERY가 제대로 작동하지 않은 상태에 대한 예외 처리
							fprintf(stderr, "QUERY ERROR\n");
							exit(0);
						}
					}
					printf("------------------------------\n");
					break; //switch문을 나가 다시 while loop을 돌아서 초기 메뉴로 이동
					
				case 4:
					printf("\n---------- TYPE IV ----------\n");
					printf("** Find the packages that were not delivered within the promised time **\n");
					strcpy(query, "with A(package_ID) as (select distinct package_ID from Delivery_Info where delivery_status = 'delay' or delivery_status = 'crash') select B.package_ID, B.package_type, B.weight, B.estimated_date from package as B, A where A.package_ID = B.package_ID");
					state = mysql_query(connection, query);
					if (state == 0) {
						sql_result = mysql_store_result(connection);
						printf("Package_ID      Package_Type    Weight          Estimated Date\n");
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							float weight = atof(sql_row[2]);
							printf("%-15s %-15s %-15.3f %-15s\n", sql_row[0], sql_row[1], weight, sql_row[3]);
						}
						mysql_free_result(sql_result);
					}
					printf("-----------------------------\n");
					break; //switch문을 나가 다시 while loop을 돌아서 초기 메뉴로 이동

				case 5:
					printf("\n----------- TYPE V -----------\n");
					printf("** Generate the Bill for each customer for the certain month **\n");
					printf("(If you wanna go back to MENU, Put 0 as input)\n");
					while (1) {
						printf("Which year: ");
						scanf("%d", &year);
						if (year == 0) break; //0을 입력 받으면 초기 메뉴로 돌아가도록
						printf("Which month: ");
						scanf("%d", &month);
						if (month == 0) break; //0을 입력 받으면 초기 메뉴로 돌아가도록

						if(month < 10) //month앞에 0이 하나 붙어야함
							sprintf(query, "select customer.name, customer.customer_ID from customer, (select distinct customer_ID from Bill where due_date like '%%%d-0%d%%') as A where A.customer_ID = customer.customer_ID", year, month);
						else //month그대로 사용해도 됨
							sprintf(query, "select customer.name, customer.customer_ID from customer, (select distinct customer_ID from Bill where due_date like '%%%d-%d%%') as A where A.customer_ID = customer.customer_ID", year, month);
						state = mysql_query(connection, query); //에러가 발생하면 state에 0이 아닌 값이 할당
						if (state == 0) {
							int num_of_customer = 0; //서로 다른 customer의 개수를 저장하는 변수
							char customer_name[100][100]; //custoemr의 이름을 저장할 공간
							char customer_ID[100][100]; //custoemr의 id를 저장할 공간

							sql_result = mysql_store_result(connection);
							if ((sql_row = mysql_fetch_row(sql_result)) == NULL) { //Query에 해당하는 결과가 없을 때
								printf("No data for %d-%d\n", year, month);
								mysql_free_result(sql_result);
								continue; //다른 year를 받기 위해 while loop을 다시 돈다
							}

							else { //Query에 해당하는 결과가 있을 때
								mysql_query(connection, query);
								sql_result = mysql_store_result(connection);
								while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
								{
									strcpy(customer_name[num_of_customer], sql_row[0]); //해당 customer들의 이름을 차례대로 저장한다
									strcpy(customer_ID[num_of_customer++], sql_row[1]); //해당 customer들의 id를 차례대로 저장한다
								}
								mysql_free_result(sql_result);

								for (int i = 0; i < num_of_customer; i++) {
									if(month < 10)
										sprintf(filename, "Bill_%d-0%d_%s.txt", year, month, customer_name[i]);
									else
										sprintf(filename, "Bill_%d-%d_%s.txt", year, month, customer_name[i]); //customer의 이름과 연도, 월을 포함한 Bill의 이름을 설정
									fp = fopen(filename, "w"); //해당 Bill 만들기

									//Bill의 상단에 customer의 개인 정보를 나타냄
									sprintf(query, "select * from customer where customer_ID = '%s'", customer_ID[i]);
									mysql_query(connection, query);
									sql_result = mysql_store_result(connection);
									
									fprintf(fp, "Customer_ID     Name      Sex        Birth             Phone Number    Address     Email\n");
									fprintf(fp, "---------------------------------------------------------------------------------------------------\n");
									while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
									{
										fprintf(fp, "%-15s     %-8s  %-7s   %-14s %-15s   %-11s   %-15s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4], sql_row[5], sql_row[6], sql_row[7]);
									}
									mysql_free_result(sql_result);

									//해당 customer가 주문한 Package의 정보에 대해 나타냄
									if(month < 10)
										sprintf(query, "with A(p_ID, p_type) as (select package.package_ID, package_type from package where package.customer_ID = '%s') select p_ID, p_type, Bill.charge, Bill.payment_method, Bill.payment_status, Bill.payment_date from Bill, A where Bill.package_ID = p_ID  and Bill.due_date like '%%%d-0%d%%'", customer_ID[i], year, month);
									else
										sprintf(query, "with A(p_ID, p_type) as (select package.package_ID, package_type from package where package.customer_ID = '%s') select p_ID, p_type, Bill.charge, Bill.payment_method, Bill.payment_status, Bill.payment_date from Bill, A where Bill.package_ID = p_ID  and Bill.due_date like '%%%d-%d%%'", customer_ID[i], year, month);
									mysql_query(connection, query);
									sql_result = mysql_store_result(connection);
									
									fprintf(fp, "\nPackage_ID     Package_Type      Charge         Pay_Method       Pay_status        Pay_date\n");
									fprintf(fp, "---------------------------------------------------------------------------------------------------------\n");
									while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
									{
										float charge = atof(sql_row[2]);
										fprintf(fp, "%-15s %-20s %-15.3f %-20s %-15s %-15s\n", sql_row[0], sql_row[1], charge, sql_row[3], sql_row[4], sql_row[5]);
									}
									mysql_free_result(sql_result);

									fclose(fp);
								}
								
								printf("\nCreating Bill for each customer Complete!!\n");
								break; //출력을 완료 했으면 초기 메뉴로 이동
							}

						}
						else { //QUERY가 제대로 작동하지 않은 상태에 대한 예외 처리
							fprintf(stderr, "QUERY ERROR\n");
							exit(0);
						}

					}

					printf("------------------------------\n");
					break; //switch문을 나가 다시 while loop을 돌아서 초기 메뉴로 이동
				default:
					printf("Wrong Number!\n\n");
			}
		}
		
	}

	return 0;
}