#ifndef __HIGHSCORE_H
#define __HIGHSCORE_H

/**
 * Stuct for userInfo - Name and Pontuation
 */
typedef struct {
	/**
	 * Name of User
	 */
	char name[30];

	/**
	 * Pontuation of User
	 */
	int pontuation;
} userInfo;

userInfo * userInf[30];
static numeroUsers = 0;

/**
 *	Create a new userInfo
 * @param name Name of User
 * @param pontuation
 * @return
 */
userInfo *create_userInfo(char *name, int pontuation) {
	userInfo *userI = malloc(sizeof(userInfo));

	strcpy(userI->name, name);
	userI->pontuation = pontuation;

	return userI;
}

/**
 * Add a new user to file userinfo.txt
 * @param dataDir path of the file
 * @param name name of the user
 * @param pont pontuation of the user
 */
void addUserInfo(char *dataDir, char *name, int pont) {
	char newLine[2] = "\n";
	char path[200];
	char line[110];
	char pontuatuion[10];
	FILE *fd;

	strcpy(path, dataDir);
	strcat(path, "/userinfo.txt");

	fd = fopen(path, "a");

	if (fd == NULL) {
		printf("ERROR opening file\n\n");
	}

	sprintf(pontuatuion, "%d", pont);
	strcat(line, name);
	strcat(line, ",");
	strcat(line, pontuatuion);

	//Write to file!!!
	fputs(newLine, fd);
	fputs(line, fd);

	int closing = fclose(fd);

	if (closing) {
		printf("error closing file \n");
	}
}


/**
 * Count number of files of file fd
 * @param fd file
 * @return integer with number of lines
 */
int countLines(FILE *fd) {
	int lines = 0;
	char ch;

	while (!feof(fd)) {
		ch = fgetc(fd);
		if (ch == '\n') {
			lines++;
		}
	}

	if (ch != '\n' && lines != 0)
		lines++;

	return lines;
}

/**
 * Sort by descending order the pontuation of users in userInf array
 */
void ordernaUsers() {
	int auxPont;
	char auxName[30];
	int i, j;

	for (i = 0; i < numeroUsers; i++) {
		for (j = i+1; j < numeroUsers; j++)

			if (userInf[i]->pontuation < userInf[j]->pontuation) {

				auxPont = userInf[j]->pontuation;
				strcpy(auxName, userInf[j]->name);

				userInf[j]->pontuation = userInf[i]->pontuation;
				strcpy(userInf[j]->name, userInf[i]->name);

				userInf[i]->pontuation = auxPont;
				strcpy(userInf[i]->name, auxName);
			}
	}

//	for (i = 0; i < numeroUsers; i++) {
//		printf("User name: %s\n", userInf[i]->name);
//		printf("Pont: %d\n", userInf[i]->pontuation);
//	}
}

/**
 * Put information about users in userInf array
 * @param dataDir path of the file
 */
void getUsersInfo(char *dataDir) {
	char line[110];
	char path[200];
	int i;
	FILE *fd;

	// Open File
	strcpy(path, dataDir);
	strcat(path, "/userinfo.txt");
	fd = fopen(path, "r");
	if (fd == NULL) {
		printf("ERROR opening file\n\n");
		return;
	}

	// Parse Lines
	i = 0;
	while (fgets(line, 100, fd) != NULL) {
		char *token;

		char user[30];
		token = strtok(line, ",");
		strcpy(user, token);

		int pontuation;
		token = strtok(NULL, ",");
		pontuation = atoi(token);

		userInf[i] = create_userInfo(user, pontuation);
		i++;
	}

	numeroUsers = i;


}

/**
 * Show top 5 best players
 */
void showBestUsersInfo(char *dataDir){

	int toShow = 5, i;

	getUsersInfo(dataDir);
	ordernaUsers();

	if(numeroUsers < toShow)
		toShow = numeroUsers;

	printf("=========== Best 5 Players ===========\n");
	for (i = 0; i < toShow; i++) {
		printf("Pont: %d  -  ", userInf[i]->pontuation);
		printf("User name: %s\n", userInf[i]->name);

	}
	printf("\nLeaving in 2 seconds...");

}

/**
 * Show all players
 */
void showAllUsers(char *dataDir){
	int i;

	getUsersInfo(dataDir);

	printf("\n\n\n");
	for (i = 0; i < numeroUsers; i++) {
		printf("Pont: %d  -  ", userInf[i]->pontuation);
		printf("User name: %s\n", userInf[i]->name);

	}
	printf("\n");

}

#endif
