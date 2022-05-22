#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> //uncomment this block to check for heap memory allocation leaks.

// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAXCOUNT 1023


typedef struct StudentCourseGrade
{
	char courseName[35];
	int grade;
}StudentCourseGrade;

typedef struct Student
{
	char name[35];
	StudentCourseGrade* grades; //dynamic array of courses
	int numberOfCourses;
}Student;


//Part A
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);

//Part B
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);
Student* readFromBinFile(const char* fileName);

int main()
{
	//Part A
	int* coursesPerStudent = NULL;
	int numberOfStudents = 0;
	char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);
	printStudentArray(students, coursesPerStudent, numberOfStudents);
	//studentsToFile(students, coursesPerStudent, numberOfStudents); //this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)

	//Part B
	Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	writeToBinFile("students.bin", transformedStudents, numberOfStudents);
	Student* testReadStudents = readFromBinFile("students.bin");

	//add code to free all arrays of struct Student


	/*_CrtDumpMemoryLeaks();*/ //uncomment this block to check for heap memory allocation leaks.
	// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

	return 0;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	
	FILE* pfile;
	*numberOfStudents = 0;
	char lineBuffer[MAXCOUNT];
	// open the file for reading
	pfile = fopen(fileName, "r");
	if (pfile == NULL)
	{
		printf("error");
		exit(1);
	}
	//count the number of students
	while (fgets(lineBuffer, MAXCOUNT, pfile) )
	{
			(* numberOfStudents)++;
	}
	rewind(pfile);
	// Assign memory to each student's courses in the array
	(* coursesPerStudent) = (int*)malloc((*numberOfStudents) * sizeof(int));
	for (int i = 0; i < *numberOfStudents; i++)
	{
		//get the next line
		fgets(lineBuffer, MAXCOUNT, pfile);
		//count the courses per student
		(* coursesPerStudent)[i] = countPipes(&lineBuffer, MAXCOUNT);
	}
	fclose(pfile);
}

int countPipes(const char* lineBuffer, int maxCount)
{
	int counter = 0;
	//check line buffer
	if (lineBuffer == NULL)
		return -1;
	//check if maxcount is legal
	if (maxCount <= 0)
		return 0;
	//count the pipes in the line
	for (int i = 0; (i < maxCount) && (*lineBuffer != NULL); i++)
	{
		if (*lineBuffer == '|')
		{
			counter++;
		}
		lineBuffer++;
	}
	return counter;
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	//add data to the array
	countStudentsAndCourses(fileName, coursesPerStudent, numberOfStudents);
	char*** students_array = (char***)malloc((*numberOfStudents) * sizeof(char**));
	if (!students_array)
	{ 
		printf("allocation error");
		exit(1); 
	}
	char buffer[MAXCOUNT];
	FILE* new_file = fopen(fileName, "r");
	if (new_file == NULL)
	{ 
		printf("can not open the file\n");
		exit(1); 
	}
	rewind(new_file);
	int i = 0;
	while (i < *numberOfStudents)
	{
		// get number of courses 
		int num_of_courses = (*(*(coursesPerStudent)+i) * 2) + 1;
		*(students_array + i) = (char**)malloc(num_of_courses * sizeof(char*));
		if (!(*(students_array + i)))
		{ 
			printf("allocation error");
			exit(1); 
		}
		// taking lines 
		fgets(buffer, MAXCOUNT, new_file);
		// remove '\n'
		char* eon;
		if ((eon = strchr(buffer, '\n')) != NULL)
		{
			*eon = '\0';
		}
		char* data;
		const char s[3] = "|,";
		// get name
		data = strtok(buffer, s);
		int j = 0;
		*(*(students_array + i) + j) = (char*)malloc((strlen(data) + 1) * sizeof(char));
		if (!(*(*(students_array + i) + j)))
		{ 
			printf("allocation error");
			exit(1); 
		}
		// copy names to array
		strcpy((*(*(students_array + i) + j)), data);
		j++;
		data = strtok(NULL, s);
		while (data != NULL)
		{
			*(*(students_array + i) + j) = (char*)malloc((strlen(data) + 1) * sizeof(char));
			if (!(*(*(students_array + i) + j)))
			{ 
				printf("allocation error"); 
				exit(1); 
			}
			// copy courses to the array
			strcpy((*(*(students_array + i) + j)), data);
			j++;
			data = strtok(NULL, s);
		}
		i++;
	}
	assert(fclose(new_file) == 0);
	return students_array;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	//check if the factor is legal
	if (factor > 20 || factor < -20)
	{
		printf("Factor is out of range ( -20 to 20 )");
		return;
	}
	int i = 0;
	while (i < numberOfStudents)
	{
		int num_of_courses = (*(coursesPerStudent + i) * 2);
		int j = 1;
		while (j < num_of_courses)
		{
			char* the_current_course = ((students + i) + j);

			// if the course exists
			if (strcmp(the_current_course, courseName) == 0)
			{
				int curremt_grade = atoi(((students + i) + j + 1));
				// add the factor
				curremt_grade = curremt_grade + factor;
				char* buffer[3];
				// copy the grade to the list
				strcpy(((students + i) + j + 1), _itoa(curremt_grade, buffer, 10));
			}
			j++;
		}
		i++;
	}
}

void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
	int i = 0;
	while(i < numberOfStudents)
	{
		printf("  name: %s\n--------------\n", students[i][0]);
		int j = 1;
		while(j <= 2 * coursesPerStudent[i])
		{
			printf("  course: %s\n", students[i][j]);
			printf("  grade: %s\n", students[i][j + 1]);
			j = j + 2;
		}
		printf("\n\n");
		i++;
	}
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	// open the file
	FILE* file = fopen("studentList.txt", "w");
	if (file == NULL)
	{
		printf("Error - can not open the file\n");
		exit(1);
	}
	rewind(file);
	int i = 0;
	while  (i < numberOfStudents)
	{
		// write name for the file
		fputs(((students + i)), file);

		// write courses for the file
		int number_of_courses = (*(coursesPerStudent + i) * 2);
		int j = 1;
		while (j < number_of_courses + 1)
		{
			char* current_course = ((students + i) + j);
			char* current_grade = ((students + i) + j + 1);

			fputc('|', file);
			fputs(current_course, file);
			fputc(',', file);
			fputs(current_grade, file);
			j = j + 2;
		}
		fputc('\n', file);
		i++;
	}
	

	assert(fclose(file) == 0);

	// release memory
	int g = 0;
	while (g < numberOfStudents)
	{
		int number_of_courses = ((*(coursesPerStudent + g)) * 2) + 1;
		int j = 0;
		while ( j < number_of_courses)
		{
			free(((students + g) + j));
			 j++;
		}
		g++;
	}

	// release  memory
	int k = 0;
	while ( k < numberOfStudents)
	{
		free(*(students + k));
		k++;
	}

	// release memory
	free(coursesPerStudent);

	// release memory
	free(students);
	
}
 
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	// open file for binary writing
	FILE* newfile = fopen("studentList.bin", "wb");
	if (newfile == NULL) 
	{ 
		printf("can not open the file\n"); 
		exit(1); 
	}

	fwrite(&numberOfStudents, sizeof(int), 1, newfile);
	int i = 0;
	while ( i < numberOfStudents)
	{
		// write student name
		fwrite(students[i].name, 35 * sizeof(char), 1, newfile);

		// write number of courses
		fwrite(&students[i].numberOfCourses, sizeof(int), 1, newfile);

		// get the courses
		int number_of_courses = students[i].numberOfCourses;
		int j = 0;
		while ( j < students[i].numberOfCourses)
		{
			// write course name
			fwrite(students[i].grades[j].courseName, 35 * sizeof(char), 1, newfile);

			// write course grade
			fwrite(&students[i].grades[j].grade, sizeof(int), 1, newfile);
			j++;
		}
		i++;
	}

	assert(fclose(newfile) == 0);
}

Student* readFromBinFile(const char* fileName)
{
	// open file for binary for reading
	FILE* newfile = fopen("studentList.bin", "rb");
	if (newfile == NULL) 
	{ 
		printf("can not open the file\n"); 
		exit(1); 
	}

	
	int number_of_students = 0;
	// get number of students
	fread(&number_of_students, sizeof(int), 1, newfile);

	Student* newstruct = (Student*)malloc(number_of_students * sizeof(Student));
	if (!newstruct) 
	{ 
		printf("allocation error"); 
		exit(1); 
	}
	int i = 0;
	while ( i < number_of_students)
	{
		// get student name
		fread(newstruct[i].name, 35 * sizeof(char), 1, newfile);

		// get student numbers of courses
		fread(&newstruct[i].numberOfCourses, sizeof(int), 1, newfile);
		newstruct[i].grades = (StudentCourseGrade*)malloc(newstruct[i].numberOfCourses * sizeof(StudentCourseGrade));
		if (!newstruct[i].grades) 
		{ 
			printf("allocation error"); 
			exit(1); 
		}

		int j = 0;
		while ( j < newstruct[i].numberOfCourses)
		{
			// write course name
			fread(newstruct[i].grades[j].courseName, 35 * sizeof(char), 1, newfile);

			// write course grade
			fread(&newstruct[i].grades[j].grade, sizeof(int), 1, newfile);
			j++;
		}
		i++;
	}

	assert(fclose(newfile) == 0);

	return newstruct;
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	Student* newstruct = (Student*)malloc(numberOfStudents * sizeof(Student));
	if (!newstruct) 
	{ 
		printf("allocation error"); 
		exit(1); 
	}
	int i = 0;
	while ( i < numberOfStudents)
	{
		// get student name
		strcpy(newstruct[i].name, ((students + i)));
		newstruct[i].numberOfCourses = *(coursesPerStudent + i);

		newstruct[i].grades = (StudentCourseGrade*)malloc(newstruct[i].numberOfCourses * sizeof(StudentCourseGrade));
		if (newstruct[i].grades == NULL) 
		{    
			printf("allocation error"); 
			exit(1); 
		}
		// get courses
		int number_of_courses = ((*(coursesPerStudent + i)) * 2);
		int j = 1;
		int k = 0;
		while ( j < number_of_courses)
		{
			strcpy(newstruct[i].grades[k].courseName, ((students + i) + j));
			newstruct[i].grades[k].grade = atoi(((students + i) + j + 1));
			j=j+2;
			k++;
		}
		i++;
	}
	return newstruct;
}
