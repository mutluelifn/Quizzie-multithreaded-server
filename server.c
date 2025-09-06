//Programmer: Elif Nalan Mutlu
//Program: CSCI3240 Final Project: Study App: Quizzie  - Quizzing Yourself!

#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>


#define MAX_SIZE 2048
#define MAX_LENGTH 25 
#define MAX_QUESTION_LENGTH 512
#define MAX_OPTION_LENGTH 216
#define MAX_USER 100


//========= Struct for Questions & Answers ======
typedef struct {
    //char user_name[MAX_LENGTH];
    char question[MAX_QUESTION_LENGTH];
    char optionA[MAX_OPTION_LENGTH];
    char optionB[MAX_OPTION_LENGTH];
    char optionC[MAX_OPTION_LENGTH];
    char optionD[MAX_OPTION_LENGTH];
    char answer[MAX_OPTION_LENGTH];

} BugBytes_Struct;

//======= Struct for User ======
typedef struct {
    char user_name[MAX_LENGTH];
    int total_questions;
    int correct_questions;
} User_Info;  




//global variables
int readcnt = 0;
sem_t mutex, w; //for semaphores


//prototypes
void addNewQuestion(const BugBytes_Struct quiz, const char *filename);
void addUser(const User_Info users, const char *filename);
void update_score(const User_Info u[], const char *filename, int ucount);
char* QuizzingUser(BugBytes_Struct quiz[], int qnum, char* correct_option, char* correct_answer_text);
char* user_report(const User_Info us[], int unum);
void serverFunction(int connfd, BugBytes_Struct quiz[], int * count, User_Info users[], int * user_count);



//========== NOTE ==============
//this project will be favoring the reader because 
//the main function is quizzing the user
//- which is focused on reader more
//==============================

///========== locking =======
//functions for locking - since it is used several times, i decided to have a function for it
void reader_entry() {
    P(&mutex);
    readcnt++;
    if (readcnt == 1)
        P(&w); // First reader locks writers
    V(&mutex);
}

//=========== Unlocking ======
//function for unlocking - since it is used several times, i decided to have a function for it
void reader_exit() {
    P(&mutex);
    readcnt--;
    if (readcnt == 0)
        V(&w); // Last reader unlocks writers
    V(&mutex);
}


//======== Adding a new question ========
void addNewQuestion(const BugBytes_Struct quiz, const char *filename) 
{    
    FILE *file = fopen(filename, "a");  // Open for appending
    if (file == NULL) {
        perror("File open failed");
        return;
    }

    //adding the informetion to the file
    fprintf(file, "%s,%s,%s,%s,%s,%s\n",
            quiz.question,
            quiz.optionA,
            quiz.optionB,
            quiz.optionC,
            quiz.optionD,
            quiz.answer);

    fclose(file);
     
}

//========= Adding a new user ===============
void addUser(const User_Info users, const char *filename) 
{    
    
    FILE *file = fopen(filename, "a");  // Open for appending
    if (file == NULL) {
        perror("File open failed");
        return;
    }
    
    fprintf(file, "%s,%d,%d\n",
            users.user_name,
            users.total_questions, 
            users.correct_questions);

    fclose(file);
     
}


//======== for debugging purposes =======
void printstuff(const BugBytes_Struct quiz[], int qnum)
{
    for(int i =0; i < qnum; ++i)
        {
            printf("%s\n", quiz[i].question);
            printf("%s\n", quiz[i].optionA);
            printf("%s\n", quiz[i].optionB);
            printf("%s\n", quiz[i].optionC);
            printf("%s\n", quiz[i].optionD);
            printf("%s\n", quiz[i].answer);
        }
}


//======= for debugging purposes ======
void printuser(const User_Info users[], int user_num)
{
    for(int i = 0; i < user_num; ++i)
        {
            printf("%s\n", users[i].user_name);
            printf("%d\n", users[i].total_questions);
            printf("%d\n", users[i].correct_questions);
        }
}


//============== Quiz Function ================
// Update the QuizzingUser function to store just answer option content
char* QuizzingUser(BugBytes_Struct quiz[], int qnum, char* correct_option, char* correct_answer_text) 
{
    // Creating a string for the question to be quizzed
    char *quizqst = malloc(MAX_SIZE * sizeof(char));
    bzero(quizqst, MAX_SIZE);

    // Set the correct option and store the correct answer text
    if (strcmp(quiz[qnum].answer, "A") == 0) {
        *correct_option = 'A';
        strcpy(correct_answer_text, quiz[qnum].optionA);
    }
    else if (strcmp(quiz[qnum].answer, "B") == 0) {
        *correct_option = 'B';
        strcpy(correct_answer_text, quiz[qnum].optionB);
    }
    else if (strcmp(quiz[qnum].answer, "C") == 0) {
        *correct_option = 'C';
        strcpy(correct_answer_text, quiz[qnum].optionC);
    }
    else if (strcmp(quiz[qnum].answer, "D") == 0) {
        *correct_option = 'D';
        strcpy(correct_answer_text, quiz[qnum].optionD);
    }
    
    snprintf(quizqst, MAX_SIZE,
             "Question: %s\nOption A: %s\nOption B: %s\nOption C: %s\nOption D: %s\n",
             quiz[qnum].question,
             quiz[qnum].optionA,
             quiz[qnum].optionB,
             quiz[qnum].optionC,
             quiz[qnum].optionD);
    return quizqst;
}


//====== User Report =============
// this function is for the report of the user, containing the name and points
// of each of the user
char* user_report(const User_Info us[], int unum)
{
    char *report = malloc(MAX_SIZE);
    if (!report) return NULL;

    float score_pct = 0.0f;
    if (us[unum].total_questions > 0) {  // Avoid division by zero
        // force float division
        score_pct = ((float)us[unum].correct_questions / us[unum].total_questions) * 100.0f;
    }

    snprintf(report, MAX_SIZE,
        "Alright %s,\n"
        "Here are your scores:\n"
        "  total questions: %d\n"
        "  correct answers: %d\n"
        "Your Total Score is: %.2f%%\n",
        us[unum].user_name,
        us[unum].total_questions,
        us[unum].correct_questions,
        score_pct
    );

    return report;

}

//============ updating the score ================
// this function updates the score after the program is done
void update_score(const User_Info u[], const char *filename, int ucount)
{
     FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Could not open scores file for writing");
        return;
    }
    
    for (int i = 0; i < ucount; i++) 
    {
        fprintf(file, "%s,%d,%d\n", 
                u[i].user_name,
                u[i].total_questions,
                u[i].correct_questions);
    }
    
    fclose(file);
}


//==== The main function ==============
void serverFunction(int connfd, BugBytes_Struct quiz[], int * count, 
User_Info users[], int * user_count)
{

//=========== PROCESSING THE FILES =========
       //extracting the data from records.csv - as impleneted in lab6
    reader_entry();
    FILE *file;  
    char line[MAX_SIZE];
    *count = 0;

    file = fopen("questions.csv", "r"); //reading the file
    if (file == NULL) 
    {
        printf("Unable to open the file.\n");
        return;
    }

    //Adding the record from records.csv to the employees structure
    while (fgets(line, sizeof(line), file)) 
    {
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]",
               quiz[*count].question, quiz[*count].optionA,
               quiz[*count].optionB, quiz[*count].optionC, quiz[*count].optionD, quiz[*count].answer);

        // Remove trailing \n or \r\n from all fields
                char* fields[] = {
                    quiz[*count].question,
                    quiz[*count].optionA,
                    quiz[*count].optionB,
                    quiz[*count].optionC,
                    quiz[*count].optionD,
                    quiz[*count].answer
                };

            for (int i = 0; i < 6; ++i) {
            fields[i][strcspn(fields[i], "\r\n")] = 0;
            }
        
       (*count)++;
    }
    fclose(file);
    reader_exit();

    //processing the user file
    reader_entry();
    FILE *file_user;  
    char line_user[MAX_SIZE];
    *user_count = 0;

    file_user = fopen("users.csv", "r"); //reading the file
    if (file_user == NULL) 
    {
        printf("Unable to open the file.\n");
        return;
    }

    //Adding the record from records.csv to the employees structure
    while (fgets(line_user, sizeof(line_user), file_user)) 
    {
        sscanf(line_user, "%[^,],%d,%d",
               users[*user_count].user_name, 
               &users[*user_count].total_questions,
        &users[*user_count].correct_questions);

        users[*user_count].user_name[
    strcspn(users[*user_count].user_name, "\r\n")
] = '\0';
        
           (*user_count)++;
    }
    fclose(file_user);
    reader_exit();
//=======================================================


    
//===== USER HANDLING ============================ 
            char buffer[MAXLINE]; //MAXLINE = 8192 defined in csapp.h
            //resetting the buffer
            bzero(buffer,MAXLINE);
            size_t n;
        
        
            User_Info temp_user;
            bzero(&temp_user, sizeof(User_Info));
            //the name of the user is sotred  
            n = read(connfd, buffer, MAXLINE);
            if(n<0)
            {
                perror("Read Error!!");
                return;
            }
            
          // Remove newline character before storing
        buffer[strcspn(buffer, "\r\n")] = '\0';
        strncpy(temp_user.user_name, buffer, MAX_LENGTH - 1);
        temp_user.user_name[MAX_LENGTH - 1] = '\0';
        printf("User connected: %s\n", temp_user.user_name);
        
        // Initialize new user stats
        temp_user.total_questions = 0;
        temp_user.correct_questions = 0;
        
        
        int current_user = -1;
        reader_entry();
        for (int i = 0; i < *user_count; i++) {
            if (strcmp(users[i].user_name, temp_user.user_name) == 0) {
                current_user = i;
                break;
            }
        }
            
        reader_exit();
            
        if (current_user < 0) {
            // brand-new user
            P(&w);
            users[*user_count] = temp_user;
            addUser(temp_user, "users.csv");
            current_user = *user_count;
            (*user_count)++;
            V(&w);
        }
   
    
//=====================================


//=========== QUIZ HANDLING ============
   bzero(buffer, MAXLINE);
      //the selection from the user  
    n = read(connfd, buffer, MAXLINE);
    if(n<0)
    {
        perror("Read Error!!");
        return;
    }
     
    while (1) {
        // Convert the buffer to an integer for switch
        int selection = atoi(buffer);
        bzero(buffer, MAXLINE);
        
        // Handle the selection
        switch(selection)
        {
            // Case 1: Add a new question
            case 1:
            {
                BugBytes_Struct temp;
                bzero(&temp, sizeof(BugBytes_Struct));

                //============= Read question ==============
                n = read(connfd, buffer, MAXLINE);
                if (n <= 0) 
                {
                    perror("Read error or client disconnected");
                    return;
                }
                strncpy(temp.question, buffer, MAX_QUESTION_LENGTH - 1);
                temp.question[MAX_QUESTION_LENGTH - 1] = '\0';
                bzero(buffer, MAXLINE);
                //=========================================
                
                //=========== Read option A =================
                n = read(connfd, buffer, MAXLINE);
                if (n <= 0) 
                {
                    perror("Read error or client disconnected");
                    return;
                }
                strncpy(temp.optionA, buffer, MAX_OPTION_LENGTH - 1);
                temp.optionA[MAX_OPTION_LENGTH - 1] = '\0';
                bzero(buffer, MAXLINE);
                //=============================================

                // =========== Read option B ==================
                n = read(connfd, buffer, MAXLINE);
                if (n <= 0) 
                {
                    perror("Read error or client disconnected");
                    return;
                }
                strncpy(temp.optionB, buffer, MAX_OPTION_LENGTH - 1);
                temp.optionB[MAX_OPTION_LENGTH - 1] = '\0';
                bzero(buffer, MAXLINE);
                //========================================
                
                
                //====== Read option C ==============
                n = read(connfd, buffer, MAXLINE);
                if (n <= 0) 
                {
                    perror("Read error or client disconnected");
                    return;
                }
                strncpy(temp.optionC, buffer, MAX_OPTION_LENGTH - 1);
                temp.optionC[MAX_OPTION_LENGTH - 1] = '\0';
                bzero(buffer, MAXLINE);
                //=====================================
                

                //============ Read option D ============
                n = read(connfd, buffer, MAXLINE);
                if (n <= 0) 
                {
                    perror("Read error or client disconnected");
                    return;
                }
                strncpy(temp.optionD, buffer, MAX_OPTION_LENGTH - 1);
                temp.optionD[MAX_OPTION_LENGTH - 1] = '\0';
                bzero(buffer, MAXLINE);
                //=======================================

                
                //========= Read the answer =============
                n = read(connfd, buffer, MAXLINE);
                if (n <= 0) 
                {
                    perror("Read error or client disconnected");
                    return;
                }
                strncpy(temp.answer, buffer, MAX_OPTION_LENGTH - 1);
                temp.answer[MAX_OPTION_LENGTH - 1] = '\0';
                bzero(buffer, MAXLINE);
                //======================================

                
                // Lock for writing to add the new question
                P(&w);
                addNewQuestion(temp, "questions.csv");
                quiz[*count] = temp;
                (*count)++;
                V(&w);
                
                char successMessage[MAXLINE] = "Record added Successfully!!\n\n\0";
                n = write(connfd, successMessage, strlen(successMessage));
                if (n <= 0){
                    perror("Write Error!!");
                    return;
                }

                // Print for server monitoring
                //printstuff(quiz, *count);
                break;
            }

            // Case 2: Quiz mode
            case 2:
            { 
                
            //============ Randomizing Questions =================
                unsigned int seed = time(NULL);
                char correct_label;
                char correct_answer_text[MAX_OPTION_LENGTH];
                
                // Main quiz loop - limit to 10 questions
               
                while(1)
                {
                    // Generate random question number
                    reader_entry(); //making sure the writing is locked while we are preparing 
                    //the quiz process with critical variables like count and quiz array
                    
                    int rd_num = 0;
                    if (*count > 0) {
                        rd_num = rand_r(&seed) % (*count);
                    }
            //==============================================================
                   
                    char *sending = QuizzingUser(quiz, rd_num, &correct_label, correct_answer_text);
                    reader_exit();
                    
                    if (!sending) {
                        perror("Failed to create quiz question");
                        break;
                    }

                    
                    // Send question to client
                    n = write(connfd, sending, strlen(sending));
                    if (n <= 0) {
                        perror("Write Error!!");
                        free(sending);
                        return;
                    }
                    
                    // Read answer - just one character
                    bzero(buffer, 2); // Clear the small buffer
                    n = read(connfd, buffer, 1); // Read exactly one byte
                    if (n <= 0) 
                    {
                        perror("Read error or client disconnected");
                        free(sending);
                        return;
                    }

                    // Print the answer received from client
                   // printf("The Answer Received from Client: %c\n", buffer[0]);
                    
                    // Check if user wants to exit quiz
                    if (buffer[0] == 'E' || buffer[0] == 'e') {
                        //if user wants to exit, we update the file before exiting the quiz mode
                        //also locking the critical part if users simultaneously try to update
                        P(&w);
                        update_score(users, "users.csv", *user_count);
                        V(&w);
                        
                        free(sending);
                        break;  // Exit the quiz loop
                    }
                    else
                    {
                        //if the user saw the question but exitted anyway, we do not count that question, so the incrementation of the question happen only the non-exit case
                    
                    P(&w); //locking for the writing
                    users[current_user].total_questions++;
                    V(&w);
                        
                    }
                    
                    // Compare answer with correct one - provide cleaner feedback
                    char result_msg[MAX_SIZE];
                    if (toupper(buffer[0]) == correct_label) 
                    {
                        sprintf(result_msg, "Correct! Good job!\n");

                        //since the question is correct, adding to the score
                        P(&w); //locking for the writing process
                        users[current_user].correct_questions++;
                        V(&w);
                        
                    } 
                    else 
                    {
                        sprintf(result_msg, "Wrong! The correct answer is %c: %s\n", 
                                correct_label, correct_answer_text);
                    }
                    
                    // Send result back to client
                    n = write(connfd, result_msg, strlen(result_msg));
                    if (n < 0)
                    {
                        perror("Write Error!!");
                        free(sending);
                        return;
                    }

                    //printf("%s,%d,%d\n",  users[current_user].user_name,  users[current_user].total_questions,  users[current_user].correct_questions);

                    
                    free(sending);

                    usleep(10000); // 100ms pause
                }
            break;
            }

            // Case 3: Show score
            case 3:
            {
               
                char * report = user_report(users, current_user);
                printf("%s\n", report);
                n = write(connfd, report, strlen(report));
                if (n < 0) {
                    perror("Write Error!!");
                    return;
                }

                free(report);
                
                break;
            }

            // Case 4: Exit
            case 4:
            {
                
                char message[MAXLINE] = "Connection Closed!!\nThank You For Trying Quizzie!<3 \n\0";
                
                n = write(connfd, message, strlen(message));
                if (n < 0) {
                    perror("Write Error!!");
                    return;
                }
                
                return; // Exit the function to close connection
            }
            
            default:
            {
                char message[MAXLINE] = "Invalid option. Please try again.\n\0";
                n = write(connfd, message, strlen(message));
                if (n < 0) {
                    perror("Write Error!!");
                    return;
                }
                break;
            }
        } // end of switch
        
        // Read next command
        n = read(connfd, buffer, MAXLINE);
        if (n <= 0) {
            perror("Read Error or client disconnected!!");
            return;
        }
    }
}


//for each of the threat
 void *thread(void *vargp)
{
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);

    BugBytes_Struct quiz[MAX_SIZE];
    int count = 0;
    User_Info users[MAX_USER];
    int user_count  = 0;

    serverFunction(connfd, quiz, &count, users, &user_count);  
    
    Close(connfd);
    printf("Client Disconnected\n");
    return NULL;
}


//stays the same
int main(int argc, char *argv[])
{

    Sem_init(&mutex, 0, 1); //initializing semphores - protecting reader_count
    Sem_init(&w, 0, 1); //initializing semaphores - for locking the writers


    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }


    listenfd = Open_listenfd(argv[1]); //wrapper function that calls getadderinfo, socket, bind, and listen functions in the server side

    //Server runs in the infinite loop.
    //To stop the server process, it needs to be killed using the Ctrl+C key.
   while (1) {
    	clientlen = sizeof(struct sockaddr_storage);

         connfdp = Malloc(sizeof(int)); //allocating new adress for each client
        *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen); //acctepting the connection
       
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname,
                                   MAXLINE,client_port, MAXLINE, 0);

        printf("Connected to (%s, %s)\n", client_hostname, client_port);

       //creating threat now
        Pthread_create(&tid, NULL, thread, connfdp);

    }
    exit(0);
    return 0;
}