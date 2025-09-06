//Programmer: Elif Nalan Mutlu
//Program: Study App: Quizzie  - Quizzing Yourself!


#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 1000
#define MAX_LENGTH 25 
#define MAX_QUESTION_LENGTH 512
#define MAX_OPTION_LENGTH 216


char name[MAX_LENGTH];

int main(int argc, char *argv[])
{
    int clientfd;  //file descriptor to communicate with the server
    char *host, *port;
    size_t n;

    char buffer[MAXLINE];
    
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
	   exit(0);
    }

    host = argv[1];
    port = argv[2];

     clientfd = Open_clientfd(host, port); //wrapper function that calls getadderinfo, socket and connect functions for client side

    
    //welcome ASCII Art
    printf("(¯`·.¸¸.·´¯`·.¸¸.·´¯)\n");
    printf("(                   )\n");
    printf("(   WELCOME TO      )\n");
    printf("(     QUIZZIE       )\n");
    printf("(                   )\n");
    printf("(                   )\n");
    printf("(_.·´¯`·.¸¸.·´¯`·.¸_)\n");


    printf("Who is this? :)  ");
    bzero(buffer, MAXLINE);

 
    
    //then getting the input from the user
    Fgets(buffer, MAXLINE, stdin);
    strcpy(name,buffer);
    name[strcspn(name, "\n")] = 0; //removing the new line
    
   //sending the name to the server
      n = write(clientfd, buffer, MAXLINE);
    if(n < 0){
        perror("Write Error!!");
        return -1;
    }

    
    printf("Hi %s !!\n", name);
    int exit = 1; //for getting out of the loop when option 4 is called
    while(exit)
    {

    
      printf("\n┌───────────────────────┐\n");
        printf("│ 1. Add a New Question │\n");
        printf("│ 2. Quiz!!             │\n");
        printf("│ 3. Show Score         │\n");
        printf("│ 4. Exit               │\n");
        printf("└───────────────────────┘\n");
        printf("Select an option: ");

    //first, resetting the selection buffer 
    bzero(buffer, MAXLINE);
    
    //then getting the input from the user
    Fgets(buffer, MAXLINE, stdin);
    int selection = atoi(buffer); //turning selection into an integer for switch

    //sending the option to the server
    n = write(clientfd, buffer, MAXLINE);
    if(n < 0){
        perror("Write Error!!");
        return -1;
    }


        switch(selection)
            {
                case 1:
                    {
                        //all the strings we will be using for adding a new question
                    char qst[MAX_QUESTION_LENGTH], optA[MAX_OPTION_LENGTH], optB[MAX_OPTION_LENGTH], 
                        optC[MAX_OPTION_LENGTH], optD[MAX_OPTION_LENGTH], ans[MAX_LENGTH];

                    //============ Question ===============
                    //first, getting the question from the user and sending it to the server
                    printf("Enter The Question: ");
                    Fgets(qst, MAX_QUESTION_LENGTH, stdin);
                    qst[strcspn(qst, "\n")] = 0; //remove the new line
                        
                    //sending the data to the server
                    n = write(clientfd, qst, strlen(qst));
                    if(n < 0){
                     perror("Write Error!!");
                     return -1;
                    }
                    //====================================
                        
                    //============= Option A ==============
                    printf("Enter Option A: "); 
                    Fgets(optA, MAX_OPTION_LENGTH, stdin);
                    optA[strcspn(optA, "\n")] = 0;  
                        
                    n = write(clientfd, optA, strlen(optA));
                    if(n < 0){
                     perror("Write Error!!");
                     return -1;
                    }
                    //==================================
                        

                    //============ Option B ==============
                    printf("Enter Option B: "); 
                    Fgets(optB, MAX_OPTION_LENGTH, stdin);
                    optB[strcspn(optB, "\n")] = 0;

                    n = write(clientfd, optB, strlen(optB));
                    if(n < 0){
                     perror("Write Error!!");
                     return -1;
                    }
                    //====================================
                        
                   
                     
                    //============ Option C =============
                    printf("Enter Option C: "); 
                    Fgets(optC, MAX_OPTION_LENGTH, stdin);
                    optC[strcspn(optC, "\n")] = 0;

                    n = write(clientfd, optC, strlen(optC));
                    if(n < 0){
                     perror("Write Error!!");
                     return -1;
                    }
                    //====================================


                        
                    //============ Option D ============
                    printf("Enter Option D: "); 
                    Fgets(optD, MAX_OPTION_LENGTH, stdin);
                    optD[strcspn(optD, "\n")] = 0;

                    n = write(clientfd, optD, strlen(optD));
                    if(n < 0){
                     perror("Write Error!!");
                     return -1;
                    }

                          
                    //====================================
                        
                    //============ Answer ============
                    printf("Enter The Correct Answer (A/B/C/D): "); 
                    Fgets(ans, MAX_LENGTH, stdin);
                    ans[strcspn(ans, "\n")] = 0;

                    //checking for the answer
                    if (strlen(ans) != 1 || 
                    (toupper(ans[0]) != 'A' && toupper(ans[0]) != 'B' && 
                    toupper(ans[0]) != 'C' && toupper(ans[0]) != 'D')) 
                    {
                    printf("Answer must be A, B, C or D. Please try again.\n");
                    Close(clientfd);
                    continue;
                    }

                    //making sure the answer is uppercase
                    ans[0] = toupper(ans[0]);
        
                    n = write(clientfd, ans, strlen(ans));
                    if(n < 0){
                     perror("Write Error!!");
                     return -1;
                    }

                    //printf("%s", ans);
                        
                    //====================================
                     
                    
                     //waiting for the message from the server.
                    //the message will be stored in buffer variable.
                    n = read(clientfd, buffer, MAXLINE);
                    if(n<0){
                    perror("Read Error!!");
                    return(-1);
                    }
                        
                    printf("\n");
                    Fputs(buffer, stdout); // Display server confirmation
                        
                    break;
                    }
                        

                    case 2: 
                    {
                      printf("\n┌─────────────────────┐\n");
                        printf("│     QUIZ MODE       │\n");
                        printf("└─────────────────────┘\n\n");
                    
                        int question_count = 0;
                        //int correct_answers = 0;
                        while (question_count < 10) // Limit to 10 questions
                        {
                            // Read quiz question and options from server
                            bzero(buffer, MAXLINE);
                            n = read(clientfd, buffer, MAXLINE);
                            if (n <= 0) 
                            {
                                perror("Read Error!!");
                                break;
                            }
                            
                            printf("----------------------------------------\n");
                            printf("Question %d:\n", question_count + 1);
                            Fputs(buffer, stdout); // Print the question/options
                            printf("----------------------------------------\n");
                            
                            // Get user answer
                             char answer;
                            int valid_input = 0;


                            //for getting a valid input
                            while (!valid_input) 
                            {
                                printf("Please type your answer (A/B/C/D) or E to exit: ");
                                fflush(stdout);
                                
                                char input_line[MAX_LENGTH];
                                bzero(input_line, MAX_LENGTH);
                                if (Fgets(input_line, MAX_LENGTH, stdin) == NULL) {
                                    perror("Input error");
                                    break;
                                }
                                
                                // Get first character and convert to uppercase
                                answer = toupper(input_line[0]);
                                
                                if (answer == 'A' || answer == 'B' || answer == 'C' 
                                    || answer == 'D' || answer == 'E') 
                                {
                                        valid_input = 1;
                                } 
                                else 
                                {
                                        printf("Invalid input. ");
                                }
                            }
                            
                            // Send answer to server (just the single character)
                            if (write(clientfd, &answer, 1) < 0) {
                                perror("Write Error");
                                break;
                            }
                            
                            // If the user requested exit, break just right after
                            if (answer == 'E') {
                                printf("Exiting quiz mode...\n");         
                                break;
                            }
                            
                            // Read result from server
                            bzero(buffer, MAXLINE);
                            n = read(clientfd, buffer, MAXLINE);
                            if (n < 0) {
                                perror("Read Error!!");
                                break;
                            }
                            
                            // Display result and increment question count
                            printf("%s\n", buffer);
                            question_count++;
    
                            usleep(100000); // 100ms pause - for user experience
                        }                
    
                        break;
                    }
                              
                    
                    //===== CASE 3 : SCORE PRINTING ======
                    case 3:
                        {
                            
                            bzero(buffer, MAXLINE);
                            
                            //reading the message from the server for the score
                             n = read(clientfd, buffer, MAXLINE);
                             if(n<0){
                             perror("Read Error!!");
                             return(-1);
                             }
                            
                             //displaying the message in buffer on the console
                             Fputs(buffer,stdout);
                             break;    
                         }
    
                    //====== CASE 4 : ENDING THE CONNECTION ======
                    case 4:
                        {
                             bzero(buffer, MAXLINE);
                             n = read(clientfd, buffer, MAXLINE);
                             if(n<0){
                             perror("Read Error!!");
                             return(-1);
                             }
    
    
                             Fputs(buffer,stdout);
                             Close(clientfd);
                            exit = 0;
                            break;
                         }
                        
            }//switch
            
    } //while
    
   
    return 0;
}
