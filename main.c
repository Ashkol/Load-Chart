/*Load&Chart
created by Adam Szkolny
April 2017*/

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <math.h>

struct Passed{
    bool isDomainEntered;
    bool isMaximum;
    bool isIntegrated;
    char nameOfChart[256];
    char nameOfXAxis[256];
    char nameOfData[5][256];
    char color[5][64];
    double minX, maxX;
}PassedOptions;


struct Loaded{
    char filename[256];
    int numberOfLines;
    int isDomainEntered;
    int isMaximum;
    int isIntegrated;
    double minX;
    double maxX;
    double x1;      //Domain
    double x2;
    double maximum;
    double resultOfIntegrating;
    double **Data;
}LoadedDefault = {"None", 0, 0, 0, 0, 0, 0, 0, 0, 0}, LoadedDefaultUser = {"User's data", 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct EditMenu{
    char MenuOption[9][256];
    char Color[6][32];

}EditMenuDefault = { {"1. Set domain to display",
                            "2. Display maximum(s)",
                            "3. Display integrated data",
                            "4. Set width and height",
                            "5. Name the chart",
                            "6. Name X axis",
                            "7. Name Data from: ",
                            "8. Choose color (accept with enter)",
                            "9. Create the chart: ",
                            },{"green", "blue", "red", "yellow","orange", "black"}};


void printStartingWindow();
void printMainMenu(WINDOW *menu, int active);
void printFilesWindow(WINDOW *menu, bool isUserEntered);
void printHelp(WINDOW *help);
void printOptionsWindow(WINDOW *menu);
void printChartEditionWindow(WINDOW *menu, int highlighted, char *filename, struct EditMenu Edition, struct Passed PassedOptions, int noChosenFile, int noColor);
void createChart(WINDOW *menu, struct Loaded *OutputData[], int numberOfYAxisDatas, struct Passed PassedOptions );
double findAndPrintMaximum(struct Loaded LoadedFile, struct Passed PassedOptions);     //Doesn't save maximum anywhere, just finds it. It's printed after return
double integrate(struct Loaded LoadedFile, struct Passed PassedOptions);
int checkForFile(char *filename, WINDOW *menu);
int countLines(char *filename);
double **loadFile(struct Loaded LoadedFile);
double **getUserData(WINDOW *menu);
double *sortBubble(double *ArrayToSort, int arrayLength);
double **sortBubble2D(struct Loaded LoadedFile);
double *eliminateRepeatingData1D(double *Array, int arrayLength);
double **eliminateRepeatingData(struct Loaded LoadedFile);
struct Passed controlEditMenu(WINDOW *menu, struct Loaded *OutputData[], struct EditMenu Edition, int filesLoaded);

char *MenuOptions[] = {"1. Load  a file", "2. Enter user's data", "3. Delete User's Data","4. Delete last file" ,"5. Create a chart"};

//char Title[] = "Load&Chart ver. 2.0";
//char TitleSub[] = "Press any key to continue";
int row, col, chartWidth = 900, chartHeight = 500;


int main(){
    WINDOW *mainMenu, *filesWindow, *optionsWindow, *chartEditionWindow;
    int keyInput, highlighted =0, i, j, mainMenuExit = 0;
    int filesLoaded = 0, numberOfFile =0;
    char tempFilename[256];
    double **pointerToFreeData;
    bool isUserDataEntered = FALSE;

    struct Loaded Loaded1 = LoadedDefault;
    struct Loaded Loaded2 = LoadedDefault;
    struct Loaded Loaded3 = LoadedDefault;
    struct Loaded Loaded4 = LoadedDefault;
    struct Loaded LoadedUser = LoadedDefaultUser;
    struct Loaded *pointerLoaded;
    struct Loaded *ChosenLoaded[] = {&Loaded1, &Loaded2, &Loaded3, &Loaded4, &LoadedUser};

    initscr();      //Initialization of the screen, required for curses
    printStartingWindow();
    cbreak();
    mainMenu = newwin(22,40,0,0);
    filesWindow = newwin(22,40,0,40);
    optionsWindow = newwin(3,80,22,0);
    chartEditionWindow = newwin(22,40,0,0);
    keypad(mainMenu, TRUE);
    keypad(chartEditionWindow, TRUE);

    while(mainMenuExit==0){
    if(highlighted>4) highlighted = 0;
    if(highlighted<0) highlighted = 4;
    wclear(mainMenu);
    printMainMenu(mainMenu, highlighted);
    printFilesWindow(filesWindow, isUserDataEntered);
    printOptionsWindow(optionsWindow);
    keyInput = wgetch(mainMenu);
        switch(keyInput){
        case KEY_UP:
            highlighted--;
            break;
        case KEY_DOWN:
            highlighted++;
            break;
        case KEY_F(1):
            clear();
            refresh();
            mainMenuExit = 1;
            break;
        case '\n':
            switch(highlighted){
                case 0:
                    box(mainMenu, ACS_VLINE, ACS_HLINE);        /*Draws a border around the screen*/
                    pointerLoaded = ChosenLoaded[numberOfFile];
                    mvwprintw(mainMenu, 19, 1, "Enter file's name with extension: .txt");
                    wmove(mainMenu,20,1);
                    curs_set(TRUE);
                    wgetnstr(mainMenu,tempFilename, sizeof(char)*255);
                    curs_set(FALSE);
                    wclear(mainMenu);
                    if (checkForFile(tempFilename, mainMenu)==0) break;
                        strcpy((*pointerLoaded).filename,tempFilename);
                        (*pointerLoaded).numberOfLines = countLines((*pointerLoaded).filename);

                        pointerToFreeData = loadFile(*pointerLoaded);
                        (*pointerLoaded).Data = pointerToFreeData;

                        (*pointerLoaded).Data = sortBubble2D(*pointerLoaded);

                        (*pointerLoaded).Data = eliminateRepeatingData(*pointerLoaded);
                        (*pointerLoaded).numberOfLines = (*pointerLoaded).Data[0][0];           //Data[0][0] contains number not repeating x-s
                        for(i=0;i<(*pointerLoaded).numberOfLines;i++){
                            (*pointerLoaded).Data[0][i]=(*pointerLoaded).Data[0][i+1];
                            (*pointerLoaded).Data[1][i]=(*pointerLoaded).Data[1][i+1];
                        }

                        (*pointerLoaded).Data[0] = realloc((*pointerLoaded).Data[0], sizeof(double)*(*pointerLoaded).numberOfLines);    //reallocation
                        (*pointerLoaded).Data[1] = realloc((*pointerLoaded).Data[1], sizeof(double)*(*pointerLoaded).numberOfLines);

                        wclrtobot(mainMenu);
                        mvwprintw(filesWindow,numberOfFile+1,10,"%s",(*pointerLoaded).filename);
                        wclrtoeol(filesWindow);
                        wrefresh(mainMenu);
                        wrefresh(filesWindow);
                        numberOfFile++;
                        filesLoaded++;
                        if(numberOfFile>3) numberOfFile=0;
                        if(isUserDataEntered == TRUE){
                            if(filesLoaded>5) filesLoaded = 5;
                        }else{
                            if(filesLoaded>4) filesLoaded=4;
                        }
                    break;
                case 1:
                    wmove(mainMenu,8,1);
                    wrefresh(mainMenu);
                    refresh();

                    LoadedUser.Data = getUserData(mainMenu);
                    LoadedUser.numberOfLines = LoadedUser.Data[0][0];
                    for(i=0;i<LoadedUser.numberOfLines;i++){
                        LoadedUser.Data[0][i]=LoadedUser.Data[0][i+1];
                        LoadedUser.Data[1][i]=LoadedUser.Data[1][i+1];
                    }

                    LoadedUser.Data[0] = realloc(LoadedUser.Data[0], sizeof(double)*LoadedUser.numberOfLines);
                    LoadedUser.Data[1] = realloc(LoadedUser.Data[1], sizeof(double)*LoadedUser.numberOfLines);

                    LoadedUser.Data = sortBubble2D(LoadedUser);

                    LoadedUser.Data = eliminateRepeatingData(LoadedUser);

                    LoadedUser.numberOfLines = LoadedUser.Data[0][0];
                    for(i=0;i<LoadedUser.numberOfLines;i++){
                        LoadedUser.Data[0][i]=LoadedUser.Data[0][i+1];
                        LoadedUser.Data[1][i]=LoadedUser.Data[1][i+1];
                    }



                    LoadedUser.Data[0] = realloc(LoadedUser.Data[0], sizeof(double)*LoadedUser.numberOfLines);
                    LoadedUser.Data[1] = realloc(LoadedUser.Data[1], sizeof(double)*LoadedUser.numberOfLines);

                    wclear(mainMenu);
                    if(filesLoaded<5&&isUserDataEntered==FALSE) filesLoaded+=1;
                    isUserDataEntered = TRUE;
                    break;
                case 2:
                    if(isUserDataEntered == TRUE){
                        filesLoaded-=1;
                        isUserDataEntered = FALSE;
                        LoadedUser = LoadedDefaultUser;
                    }
                    break;
                case 3:

                    if(filesLoaded!=0){
                        if(isUserDataEntered ==TRUE&&filesLoaded>1){
                            for(j=0; j<2; j++){
                            free((*ChosenLoaded[filesLoaded-2]).Data[j]);
                            }
                            ChosenLoaded[filesLoaded-2] = &LoadedDefault;
                            wmove(filesWindow, filesLoaded-1, 10);
                            wclrtoeol(filesWindow);
                            box(filesWindow, ACS_VLINE, ACS_HLINE);
                            filesLoaded-=1;
                            numberOfFile-=1;
                        }else if(isUserDataEntered==FALSE){
                            for(j=0; j<2; j++){
                            free((*ChosenLoaded[filesLoaded-1]).Data[j]);
                            }
                            ChosenLoaded[filesLoaded-1] = &LoadedDefault;
                            wmove(filesWindow, filesLoaded, 10);
                            wclrtoeol(filesWindow);
                            box(filesWindow, ACS_VLINE, ACS_HLINE);
                            filesLoaded-=1;
                            numberOfFile-=1;
                            wrefresh(filesWindow);
                        }
                    }
                    break;
                case 4:
                    if(filesLoaded!=0){
                            if(isUserDataEntered==TRUE){
                                pointerLoaded = ChosenLoaded[filesLoaded-1];
                                ChosenLoaded[filesLoaded-1] = &LoadedUser;
                            }
                    controlEditMenu(chartEditionWindow, ChosenLoaded, EditMenuDefault, filesLoaded);    //createChart() is inside this function
                    if(isUserDataEntered==TRUE){
                        ChosenLoaded[filesLoaded-1] = pointerLoaded;
                    }
                    wclear(mainMenu);
                    }else{
                    mvwprintw(mainMenu,19,1,"No files Loaded.\n Press any key to continue.");
                    box(mainMenu, ACS_VLINE, ACS_HLINE );
                    wrefresh(mainMenu);
                    getch();
                    }
                    break;
                default:
                    break;
                }
        break;
        default:
        break;
        }
    }

    if(isUserDataEntered==TRUE){
                                ChosenLoaded[filesLoaded-1] = &LoadedUser;
                            }
    for(i=0;i<filesLoaded;i++){             //Freeing memory allocated for files
        for(j=0; j<2; j++){
        free((*ChosenLoaded[i]).Data[j]);
        }
    }

    endwin();   //Ending screen mode from curses

    return 0;
}

void printStartingWindow(){
    char Title[] = "Load&Chart ver. 2.0";
    char TitleSub[] = "Press any key to continue";
    char Author[] = "by Adam Szkolny";

    initscr();
    keypad(stdscr, TRUE);
    curs_set(FALSE);     //Disables showing cursor
    getmaxyx(stdscr,row,col);
    mvprintw(row/2,(col-strlen(Title))/2,"%s", Title);
    mvprintw((row/2)+2,(col-strlen(TitleSub))/2,"%s", TitleSub);
    mvprintw(row-1,0,"%s", Author);
    refresh();
    getch();
    clear();
    endwin();
}

void printMainMenu(WINDOW *menu, int highlighted){

    int i, y=1;
    box( menu, ACS_VLINE, ACS_HLINE );

    for(i = 0; i < 5; ++i)
	{	if(highlighted == i )  /* Highlight the present choice */
		{	wattron(menu, A_REVERSE);
			mvwprintw(menu, y, 1, "%s", MenuOptions[i]);
			wattroff(menu, A_REVERSE);
		}
		else
			mvwprintw(menu, y, 1, "%s", MenuOptions[i]);
		++y;
	}

    refresh();
    wrefresh(menu);
}

void printChartEditionWindow(WINDOW *menu, int highlighted, char *filename, struct EditMenu Edition, struct Passed PassedOptions, int noChosenFile, int noColor){

    int i, j=0;
    wclear(menu);
    box(menu, ACS_VLINE, ACS_HLINE );

    for(i = 0; i < 14; ++i){
        if(highlighted==i){
            if( i==0 || i==1 || i==2 || i==3 || i==6 || i==7 || i==8 || i==10 || i==13){  /* Highlight the present choice */
                wattron(menu, A_REVERSE);
                mvwprintw(menu, i+1, 1, "%s", Edition.MenuOption[j]);
                if(PassedOptions.isDomainEntered==TRUE && i==0)wprintw(menu, " ON");
                if(PassedOptions.isDomainEntered==FALSE && i==0)wprintw(menu, " OFF");
                if(PassedOptions.isMaximum==TRUE && i==1)wprintw(menu, " ON");
                if(PassedOptions.isMaximum==FALSE && i==1)wprintw(menu, " OFF");
                if(PassedOptions.isIntegrated==TRUE && i==2)wprintw(menu, " ON");
                if(PassedOptions.isIntegrated==FALSE && i==2)wprintw(menu, " OFF");
                wattroff(menu, A_REVERSE);
                j++;
            }else if(i==4){
                wattron(menu, A_REVERSE);
                mvwprintw(menu, i+1, 1, "Width <%d>", chartWidth);
                wattroff(menu, A_REVERSE);
            }else if(i==5){
                wattron(menu, A_REVERSE);
                mvwprintw(menu, i+1, 1, "Height <%d>", chartHeight);
                wattroff(menu, A_REVERSE);
            }else if(i==9){
                wattron(menu, A_REVERSE);
                mvwprintw(menu, i+1, 1, "<%s>", filename);
                wattroff(menu, A_REVERSE);
            }else if(i==11){
                wattron(menu, A_REVERSE);
                mvwprintw(menu, i+1, 1, "<%s>", filename);
                wattroff(menu, A_REVERSE);
            }else if(i==12){
                wattron(menu, A_REVERSE);
                mvwprintw(menu, i+1, 1, "<%s>", Edition.Color[noColor]);
                wattroff(menu, A_REVERSE);
            }
        }else if( i==0 || i==1 || i==2 || i==3 || i==6 || i==7 || i==8 || i==10 || i==13){  /* Highlight the present choice */
                mvwprintw(menu, i+1, 1, "%s", Edition.MenuOption[j]);
                if(PassedOptions.isDomainEntered==TRUE && i==0)wprintw(menu, " ON");
                if(PassedOptions.isDomainEntered==FALSE && i==0)wprintw(menu, " OFF");
                if(PassedOptions.isMaximum==TRUE && i==1)wprintw(menu, " ON");
                if(PassedOptions.isMaximum==FALSE && i==1)wprintw(menu, " OFF");
                if(PassedOptions.isIntegrated==TRUE && i==2)wprintw(menu, " ON");
                if(PassedOptions.isIntegrated==FALSE && i==2)wprintw(menu, " OFF");
                j++;
            }else if(i==4){
                mvwprintw(menu, i+1, 1, "Width <%d>", chartWidth);
            }else if(i==5){
                mvwprintw(menu, i+1, 1, "Height <%d>", chartHeight);
            }else if(i==9){
                mvwprintw(menu, i+1, 1, "<%s>", filename);
            }else if(i==11){
                mvwprintw(menu, i+1, 1, "<%s>", filename);
            }else if(i==12){
                wmove(menu,i+1,1); wclrtoeol(menu); box( menu, ACS_VLINE, ACS_HLINE );
                mvwprintw(menu, i+1, 1, "<%s>", Edition.Color[noColor]);
            }


    }
    refresh();
    wrefresh(menu);
}

void printFilesWindow(WINDOW *menu, bool isUserEntered){

    box( menu, ACS_VLINE, ACS_HLINE );
    mvwprintw(menu,1,1,"File 1: ");
    mvwprintw(menu,2,1,"File 2: ");
    mvwprintw(menu,3,1,"File 3: ");
    mvwprintw(menu,4,1,"File 4: ");
    mvwprintw(menu,5,1,"User's data entered: ");
    if(isUserEntered==TRUE) wprintw(menu, "Yes");
    else wprintw(menu, "No ");

    wrefresh(menu);
}

void printHelp(WINDOW *help){

    box(help, ACS_VLINE, ACS_HLINE);
    mvwprintw(help, 1, 1, "Help");
    wrefresh(help);
}

void printOptionsWindow(WINDOW *menu){

    box( menu, ACS_VLINE, ACS_HLINE );
    mvwprintw(menu,1,1,"Press F1 to exit");
    wrefresh(menu);
    refresh();
}

int checkForFile(char *filename, WINDOW *menu){

    FILE *file;
    if((file = fopen(filename, "r"))==NULL){
        fclose(file);
        mvwprintw(menu,1,1,"File not found in the directory.\n Press any key.");
        box( menu, ACS_VLINE, ACS_HLINE );
        wrefresh(menu);
        getch();
        return 0;
    }
    else{
        fclose(file);
        return 1;
    }
}

int countLines(char *filename){

    int lines =0;
    char c=0;

    FILE *file;
    file = fopen(filename, "r");

    while(c!=EOF){
        c = fgetc(file);

        if(c == '\n') lines++;
    }

    lines -= 2;     //First two lines aren't numbers
    fclose(file);
    return lines;
}

double** loadFile(struct Loaded LoadedFile){

    double **Data;
    int c, i;

    FILE *file;
    file = fopen(LoadedFile.filename, "r");

    Data = (double**)calloc(2,sizeof(double));
    for(i=0;i<2;i++)
    {
        Data[i]=(double*)calloc(LoadedFile.numberOfLines, sizeof(double));
    }

    for(i=0; i<2; i++){
        do{
            c = fgetc(file);
        }while(c!='\n');
    }

    for(i=0;i<(LoadedFile.numberOfLines);i++){
        fscanf(file, "\t%*f\t%lf", &Data[1][i]);
        Data[1][i]=fabs(Data[1][i]);
        fscanf(file, "\t%*f\t%*f\t%lf\t%*f\n", &Data[0][i]);
    }

    fclose(file);
    return Data;
}

double **sortBubble2D(struct Loaded LoadedFile){

    int i,j;
    double temp;
    int arrLength = LoadedFile.numberOfLines;

    for(j=1;j<arrLength;j++){
        for(i=0;i<arrLength-j;i++){
            if(LoadedFile.Data[0][i]>LoadedFile.Data[0][i+1]){
                temp = LoadedFile.Data[1][i];
                LoadedFile.Data[1][i] = LoadedFile.Data[1][i+1];
                LoadedFile.Data[1][i+1] = temp;
                temp = LoadedFile.Data[0][i];
                LoadedFile.Data[0][i] = LoadedFile.Data[0][i+1];
                LoadedFile.Data[0][i+1] = temp;
            }
        }
    }
    return LoadedFile.Data;
}

double *sortBubble(double *ArrayToSort, int arrayLength){

    int i,j;
    double temp;

    for(j=1;j<arrayLength;j++){
        for(i=0;i<arrayLength-j;i++){
            if(ArrayToSort[i]>ArrayToSort[i+1]){
                temp = ArrayToSort[i];
                ArrayToSort[i] = ArrayToSort[i+1];
                ArrayToSort[i+1] = temp;
            }
        }
    }
    return ArrayToSort;
}

double **eliminateRepeatingData(struct Loaded LoadedFile){

    int i, j=0, numberOfValues=1, notRepeatingData=0;
    double temp;

    for(i=0; i<LoadedFile.numberOfLines-1; i++){                      //Set notRepeatingData
        if(LoadedFile.Data[0][i]!=LoadedFile.Data[0][i+1]){
            notRepeatingData++;
        }
    }
    notRepeatingData++;


    //Set mean
    j=0;
    for(i=0; i<LoadedFile.numberOfLines-1; i++){
        if(LoadedFile.Data[0][i]==LoadedFile.Data[0][i+1]){     /*UNADDRESSABLE ACCESS beyond heap bounds*/
            if(numberOfValues == 1)LoadedFile.Data[1][j] = LoadedFile.Data[1][i];
            LoadedFile.Data[1][j]+=LoadedFile.Data[1][i+1];
            numberOfValues++;
        }else{
            if(numberOfValues == 1)LoadedFile.Data[1][j] = LoadedFile.Data[1][i];
            LoadedFile.Data[1][j] = LoadedFile.Data[1][j]/numberOfValues;
            numberOfValues=1;
            j++;
        }
    }
    if(numberOfValues == 1)LoadedFile.Data[1][j] = LoadedFile.Data[1][i];
            LoadedFile.Data[1][j] = LoadedFile.Data[1][j]/numberOfValues;
            numberOfValues=1;
            j++;
    LoadedFile.Data[1][j] = LoadedFile.Data[1][j]/numberOfValues;

    temp = LoadedFile.Data[0][0];                           //Get the x-s right
    j=1;
    for(i=0; i<LoadedFile.numberOfLines; i++){
        if(LoadedFile.Data[0][i]!=temp){
            temp = LoadedFile.Data[0][i];
            LoadedFile.Data[0][j]=LoadedFile.Data[0][i];
            j++;
        }
    }

    realloc(LoadedFile.Data[0], sizeof(double)*(notRepeatingData+1));
    realloc(LoadedFile.Data[1], sizeof(double)*(notRepeatingData+1));

    for(i=0;i<notRepeatingData;i++){                                                        //Move array's content one cell further
        LoadedFile.Data[0][notRepeatingData-i] = LoadedFile.Data[0][notRepeatingData-i-1];
        LoadedFile.Data[1][notRepeatingData-i] = LoadedFile.Data[1][notRepeatingData-i-1];
    }

    LoadedFile.Data[0][0] = notRepeatingData;                                               //Cell [0][0] contains number of not repeating x-s
    LoadedFile.Data[1][0] = 0;

    return LoadedFile.Data;
}

double *eliminateRepeatingData1D(double *Array, int arrayLength){
    int i=0, j=0, notRepeatingData = 0;
    double temp;

    for(i=0; i<arrayLength-1; i++){                      //Set notRepeatingData
        if(Array[i]!=Array[i+1]){
            notRepeatingData++;
        }
    }

    notRepeatingData++;         //correct

    temp = Array[0];                           //Get the x-s right
    j=1;
    for(i=0; i<arrayLength; i++){
        if(Array[i]!=temp){
            temp =Array[i];
            Array[j]=Array[i];
            j++;
        }
    }

     Array = realloc(Array, sizeof(double)*(notRepeatingData+1));


    for(i=0;i<notRepeatingData;i++){                            //Move array's content one cell further
    Array[notRepeatingData-i] = Array[notRepeatingData-i-1];
    }

    Array[0] = notRepeatingData;

    return Array;
}

void createChart(WINDOW *menu, struct Loaded *OutputData[], int numberOfYAxisDatas, struct Passed PassedOptions) {


    int i=0, k=0, numberOfXs=0;
    int *pointerY;
	char name[256];
	double maxX, minX, temp;
	double *ArrayOfXs;
	int a=0, b=0, c=0, d=0, e=0;
	int MoveArray[] = {0,0,0,0,0};

	// FILE *filetest = fopen("test.txt", "w");
   // fprintf(filetest, "%f\n", (*OutputData[numberOfYAxisDatas-1]).Data[1][((*OutputData[numberOfYAxisDatas-1]).numberOfLines)-1]);
   // fclose(filetest);

    box(menu, ACS_VLINE, ACS_HLINE);
	mvwprintw(menu,19,1,"Name created file: ");
	wmove(menu,20,1);
	curs_set(TRUE);
    wgetnstr(menu, name, sizeof(char)*255);
    if(name[0]=='\n')strcpy(name, "Load&Chart.html");
    curs_set(FALSE);

    ArrayOfXs = (double*)calloc(1,sizeof(double));

    for(k=0; k<numberOfYAxisDatas; k++){
        ArrayOfXs = realloc(ArrayOfXs, sizeof(double)*((*OutputData[k]).numberOfLines+numberOfXs));
        for(i=0; i<(*OutputData[k]).numberOfLines; i++){
            ArrayOfXs[i+numberOfXs] = (*OutputData[k]).Data[0][i];
        }
        numberOfXs += (*OutputData[k]).numberOfLines;
    }

    ArrayOfXs = sortBubble(ArrayOfXs, numberOfXs);
    ArrayOfXs = eliminateRepeatingData1D(ArrayOfXs, numberOfXs);

    numberOfXs = ArrayOfXs[0];
    for(i=0;i<numberOfXs;i++){
        ArrayOfXs[i]=ArrayOfXs[i+1];
    }

    ArrayOfXs = realloc(ArrayOfXs, sizeof(double)*numberOfXs);

    if(PassedOptions.isDomainEntered){
            minX=PassedOptions.minX;
            maxX=PassedOptions.maxX;
        if (maxX<minX){
            temp = minX;
            minX = maxX;
            maxX = temp;
        }
    }

	FILE *file;
	file = fopen(name,"w");
	fprintf(file, "<!DOCTYPE html>\n");
	fprintf(file, "<html>\n");
	fprintf(file, "<head>\n");
	fprintf(file, "<title>My Google chart</title>\n");
	fprintf(file, "<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>\n");
	fprintf(file, "<script type = \"text/javascript\">\n");
	fprintf(file, "  google.load('visualization', '1.1', { packages: ['corechart'] });\n");
	fprintf(file, "  google.setOnLoadCallback(drawChart);\n");
	fprintf(file, "function drawChart() {\n");
	fprintf(file, "var data = new google.visualization.DataTable();\n");
    fprintf(file, "data.addColumn('number', '%s');\n", PassedOptions.nameOfXAxis);
    fprintf(file, "data.addColumn('number', '%s');\n", PassedOptions.nameOfData[0]);
    if(numberOfYAxisDatas>1) fprintf(file, "data.addColumn('number', '%s');\n", PassedOptions.nameOfData[1]);
    if(numberOfYAxisDatas>2) fprintf(file, "data.addColumn('number', '%s');\n", PassedOptions.nameOfData[2]);
    if(numberOfYAxisDatas>3) fprintf(file, "data.addColumn('number', '%s');\n", PassedOptions.nameOfData[3]);
    if(numberOfYAxisDatas>4) fprintf(file, "data.addColumn('number', '%s');\n", PassedOptions.nameOfData[4]);
    fprintf(file, "data.addRows([\n");

	if(PassedOptions.isDomainEntered){
        if(minX<ArrayOfXs[0]) minX = ArrayOfXs[0];          //if minX entered by user is smaller than value of the first cell
       for(i=0; i<numberOfYAxisDatas; i++){                 //minX' value becomes equal to value of ArrayOfXs[0]
            while((*OutputData[i]).Data[0][MoveArray[i]]<minX) MoveArray[i]+=1;
       }
	}

	if(PassedOptions.isDomainEntered){                                      //if maxX entered by user is smaller than value of the first cell
        if(maxX>ArrayOfXs[numberOfXs-1]) maxX = ArrayOfXs[numberOfXs-1];     //maxX' value becomes equal to value of ArrayOfXs[0]
	}

	for(i=0;i<numberOfXs;i++){
        if(PassedOptions.isDomainEntered == FALSE || (PassedOptions.isDomainEntered==TRUE&&ArrayOfXs[i]>=minX&&ArrayOfXs[i]<=maxX)){
            fprintf(file,"[%f", ArrayOfXs[i] );   //data to chart

            for(k=0; k<numberOfYAxisDatas;k++){
                switch(k){
                    case 0:
                        pointerY = &a;
                        break;
                    case 1:
                        pointerY = &b;
                        break;
                    case 2:
                        pointerY = &c;
                        break;
                    case 3:
                        pointerY = &d;
                        break;
                    case 4:
                        pointerY = &e;
                        break;
            }
                if(ArrayOfXs[i] == (*OutputData[k]).Data[0][(*pointerY)+MoveArray[k]]){
                    fprintf(file,", %lf", (*OutputData[k]).Data[1][(*pointerY)+MoveArray[k]]);
                        switch(k){
                        case 0:
                            a++;
                            break;
                        case 1:
                            b++;
                            break;
                        case 2:
                            c++;
                            break;
                        case 3:
                            d++;
                            break;
                        case 4:
                            e++;
                            break;
                        }
                }else{
                fprintf(file, ", null");
                }
            }

            fputs("],\n", file);
		}
	}

	fprintf(file,"\n]);\n");

	fprintf(file, "var options = {\n");
	fprintf(file, "title: '%s',\n",PassedOptions.nameOfChart);
	fprintf(file, "interpolateNulls: true,\n");
	fprintf(file, "legend: {position: 'right'},\n");
	fprintf(file, "colors: ['%s', '%s', '%s', '%s', '%s']\n", PassedOptions.color[0], PassedOptions.color[1], PassedOptions.color[2], PassedOptions.color[3], PassedOptions.color[4]);
	fprintf(file, "}\n");
	fprintf(file, "var chart = new google.visualization.LineChart(document.getElementById('linechart_material'));\n");
	fprintf(file, "chart.draw(data, options);\n");
	fprintf(file, "};\n");
	fprintf(file, "</script>\n");
	fprintf(file, "</head>\n");
	fprintf(file, "<body>\n");
	fprintf(file, "<div id=\"linechart_material\"  style=\"width: %dpx; height: %dpx\"></div>\n", chartWidth, chartHeight);
	for(k=0; k<numberOfYAxisDatas;k++){

        fprintf(file, "File: %s  ", (*OutputData[k]).filename);
        if(PassedOptions.isMaximum == TRUE){
            fprintf(file, "Maximum = %f  ", findAndPrintMaximum(*OutputData[k], PassedOptions));
        }
        if(PassedOptions.isIntegrated == TRUE){
            fprintf(file, "Result of integrating = %f", integrate(*OutputData[k], PassedOptions));
        }
        fprintf(file, "<br/>");
	}

	fprintf(file, "</body>\n");
	fprintf(file, "</html>\n");



	fclose(file);
	free(ArrayOfXs);        //Freeing memory allocated for array in createChart();
}

double **getUserData(WINDOW *menu){

    int exit = 1, i=0; int j;
    double **Data;

    curs_set(TRUE);
    wclear(menu);
    box( menu, ACS_VLINE, ACS_HLINE);
    wrefresh(menu);

    Data=(double**)calloc(2,sizeof(double));
    for(i=0;i<2;i++){
        Data[i]=(double*)calloc(2,sizeof(double));
    }

    i=0;

    do{
        wmove(menu,1,1);
        wclrtobot(menu);
        for(j=0; j<i; j++){
            mvwprintw(menu,4+j, 1, "x=%g\t y=%g\n", Data[0][j+1], Data[1][j+1]);
        }
        box( menu, ACS_VLINE, ACS_HLINE);
        wrefresh(menu);

        mvwprintw(menu,1,1,"Enter x and y separated with space.");
        mvwprintw(menu,2,1,"If input is wrong data is saved.");

        if(mvwscanw(menu, 3,1,"%lf %lf", &Data[0][i+1], &Data[1][i+1])==2){
            ;   //Without semicolon program shuts down - oh, it's because after mvscanw there's no semicolon!
            i++;
            Data[0] = realloc(Data[0],sizeof(double)*(i+2));
            Data[1] = realloc(Data[1],sizeof(double)*(i+2));

        }else if(i!=0){
            curs_set(FALSE);
            box(menu, ACS_VLINE, ACS_HLINE);
            wmove(menu,1,1);
            mvwprintw(menu, 19,1,"Data's been saved.");
            fflush(stdin);
            mvwprintw(menu, 20,1,"Press any key to proceed");
            wrefresh(menu);
            getch();
            exit =0;
        }

    if(i==14) i=0;

    }while(exit);
    fflush(stdin);

    Data[0][0]=i;


    return Data;
}

struct Passed controlEditMenu(WINDOW *menu, struct Loaded *OutputData[], struct EditMenu Edition, int filesLoaded){
    int keyInput, highlighted =0, exit=1, noChosenFile = 0, noColor = 0;
    struct Passed ReturnOptions = {FALSE, FALSE, FALSE, "default","default",{"default","default","default","default", "default"},{"green","green","green","green", "green"},0,0};
    keypad(menu, TRUE);

    while(exit){
    curs_set(FALSE);
    if (highlighted>13) highlighted = 0;
    if (highlighted<0) highlighted = 13;
    wclear(menu);
    printChartEditionWindow(menu, highlighted, (*OutputData[noChosenFile]).filename, Edition, ReturnOptions,  noChosenFile, noColor);

    keyInput = wgetch(menu);
    if(keyInput == KEY_UP) highlighted--;
    if(keyInput == KEY_DOWN) highlighted++;
    if(keyInput == '\n'){
        switch(highlighted){
        case 0:
            curs_set(TRUE);
            box(menu, ACS_VLINE, ACS_HLINE);
            wrefresh(menu);
            if(ReturnOptions.isDomainEntered == FALSE){
                ReturnOptions.isDomainEntered = TRUE;
                mvwprintw(menu,19,1,"Enter first edge of domain: ");
                while(wscanw(menu, "%lf", &ReturnOptions.minX)!=1){
                    mvwprintw(menu, 18, 1, "Wrong input");
                    wmove(menu, 19, 29);
                    wclrtoeol(menu);
                    box(menu, ACS_VLINE, ACS_HLINE);
                }
                wmove(menu, 18, 1);
                wclrtoeol(menu);
                box(menu, ACS_VLINE, ACS_HLINE);
                mvwprintw(menu,20,1,"Enter second edge of domain: ");
                while(wscanw(menu, "%lf", &ReturnOptions.maxX)!=1){
                    mvwprintw(menu, 18, 1, "Wrong input");
                    wmove(menu, 20, 30);
                    wclrtoeol(menu);
                    box(menu, ACS_VLINE, ACS_HLINE);
                }
            }else ReturnOptions.isDomainEntered = FALSE;
            break;
        case 1:
            if(ReturnOptions.isMaximum == FALSE) ReturnOptions.isMaximum = TRUE;
            else ReturnOptions.isMaximum = FALSE;
            break;
        case 2:
            if(ReturnOptions.isIntegrated == FALSE) ReturnOptions.isIntegrated = TRUE;
            else ReturnOptions.isIntegrated = FALSE;
            break;
        case 6:
            box(menu, ACS_VLINE, ACS_HLINE);
            curs_set(TRUE);
            wmove(menu,20,1);
            wgetnstr(menu, ReturnOptions.nameOfChart, sizeof(char)*255);
            wclear(menu);
            break;
        case 7:
            box(menu, ACS_VLINE, ACS_HLINE);
            curs_set(TRUE);
            wmove(menu,20,1);
            wgetnstr(menu, ReturnOptions.nameOfXAxis, sizeof(char)*255);
            wclear(menu);
            break;
        case 8:
            box(menu, ACS_VLINE, ACS_HLINE);
            curs_set(TRUE);
            wmove(menu,20,1);
            wgetnstr(menu, ReturnOptions.nameOfData[noChosenFile], sizeof(char)*255);
            wclear(menu);
            break;
        case 9:
            box(menu, ACS_VLINE, ACS_HLINE);
            curs_set(TRUE);
            wmove(menu,20,1);
            wgetnstr(menu, ReturnOptions.nameOfData[noChosenFile], sizeof(char)*255);
            wclear(menu);
            break;
        case 10:
            strcpy(ReturnOptions.color[noChosenFile], Edition.Color[noColor]);
            break;
        case 11:;
            strcpy(ReturnOptions.color[noChosenFile], Edition.Color[noColor]);
            break;
        case 12:
            strcpy(ReturnOptions.color[noChosenFile], Edition.Color[noColor]);
            break;
        case 13:
            curs_set(TRUE);
            createChart(menu, OutputData, filesLoaded, ReturnOptions);
            break;
        default:
            break;
        }
    }
    if(keyInput == KEY_LEFT){
        switch(highlighted){
        case 4:
            if(chartWidth>100) chartWidth -= 10;
            break;
        case 5:
            if(chartHeight>100) chartHeight -= 10;
            break;
        case 9:
            noChosenFile--;
            if (noChosenFile<0) noChosenFile = filesLoaded-1;
            break;
        case 11:
            noChosenFile--;
            if (noChosenFile<0) noChosenFile = filesLoaded-1;
            break;
        case 12:
            noColor--;
            if (noColor<0) noColor = 5;
            break;
        }
    }
    if(keyInput == KEY_RIGHT){
        switch(highlighted){
        case 4:
            if(chartWidth<3200) chartWidth += 10;
            break;
        case 5:
            if(chartHeight<3200) chartHeight += 10;
            break;
        case 9:
            noChosenFile++;
            if (noChosenFile>filesLoaded-1) noChosenFile = 0;
            break;
        case 11:
            noChosenFile++;
            if (noChosenFile>filesLoaded-1) noChosenFile = 0;
            break;
        case 12:
            noColor++;
            if (noColor>5) noColor = 0;
            break;
        }
    }
    if(keyInput == KEY_F(1)) exit = 0;
    }

    return ReturnOptions;
}

double findAndPrintMaximum(struct Loaded LoadedFile, struct Passed PassedOptions){

    int i;
    double maxY;

    maxY=LoadedFile.Data[1][0];

    for(i=0;(i<LoadedFile.numberOfLines);i++){
        if(PassedOptions.isDomainEntered==TRUE){
            if(PassedOptions.minX<=LoadedFile.Data[0][i] && LoadedFile.Data[0][i]<=PassedOptions.maxX){
                if(LoadedFile.Data[1][i]>maxY) maxY = LoadedFile.Data[1][i];
            }
        }else{
            if(LoadedFile.Data[1][i]>maxY) maxY = LoadedFile.Data[1][i];
        }
    }

    return maxY;
}

double integrate(struct Loaded LoadedFile, struct Passed PassedOptions){
    int i;
    double sum=0;


    for(i=0;i<LoadedFile.numberOfLines-1;i++){
        if(PassedOptions.isDomainEntered==TRUE){
            if(PassedOptions.minX<=LoadedFile.Data[0][i] && LoadedFile.Data[0][i+1]<=PassedOptions.maxX){
            sum+=(LoadedFile.Data[1][i]+LoadedFile.Data[1][i+1])*fabs((LoadedFile.Data[0][i+1]-LoadedFile.Data[0][i]))/2;
            }
        }else sum+=(LoadedFile.Data[1][i]+LoadedFile.Data[1][i+1])*fabs((LoadedFile.Data[0][i+1]-LoadedFile.Data[0][i]))/2;
    }

    return sum;
}

