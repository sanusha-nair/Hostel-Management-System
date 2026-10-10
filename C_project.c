
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 10

struct Student {
    int id;
    char name[20];
    int roomno;
};

struct Meal {
    int id;
    char name[30];
    char type[30];
    int prepared;
    int consumed;
    float rating;
};

struct Parcel {
    int parcelId;
    int regid;
    char courier[30];
    char arrivalDate[15];
    int status;
};

struct Complaint {
    int complaintId;
    int studentId;
    int roomNo;
    char category[30];
    char description[100];
    int priority;
    int status;
};

struct Student *std;
struct Meal *meals;
struct Parcel *parcels;
struct Complaint *complaints;

int studentCount = 0, studentCapacity = MAX;
int mealCount = 0, mealCapacity = MAX;
int parcelCount = 0, parcelCapacity = MAX;
int complaintCount = 0, complaintCapacity = MAX;

int ensureCapacity(void **arr, int *capacity, size_t size) {
    void *temp = realloc(*arr, (size_t)(*capacity * 2) * size);

    if (temp == NULL) {
        printf("Memory allocation failed.\n");
        return 0;
    }

    *arr = temp;
    *capacity *= 2;
    return 1;
}

struct Student *searchStudent(int id) {
    for (int i = 0; i < studentCount; i++)
        if (std[i].id == id)
            return &std[i];

    return NULL;
}

struct Meal *searchMeal(int id) {
    for (int i = 0; i < mealCount; i++)
        if (meals[i].id == id)
            return &meals[i];

    return NULL;
}

struct Parcel *searchParcel(int id) {
    for (int i = 0; i < parcelCount; i++)
        if (parcels[i].parcelId == id)
            return &parcels[i];

    return NULL;
}

struct Complaint *searchComplaint(int id) {
    for (int i = 0; i < complaintCount; i++)
        if (complaints[i].complaintId == id)
            return &complaints[i];

    return NULL;
}

void saveStudents(void) {
    FILE *fp = fopen("students.txt", "w");

    if (fp == NULL) {
        printf("Unable to save students.\n");
        return;
    }

    for (int i = 0; i < studentCount; i++)
        fprintf(fp, "%d|%s|%d\n",
                std[i].id, std[i].name, std[i].roomno);

    fclose(fp);
}

void saveMeals(void) {
    FILE *fp = fopen("meals.txt", "w");

    if (fp == NULL) {
        printf("Unable to save meals.\n");
        return;
    }

    for (int i = 0; i < mealCount; i++)
        fprintf(fp, "%d|%s|%s|%d|%d|%.2f\n",
                meals[i].id, meals[i].name, meals[i].type,
                meals[i].prepared, meals[i].consumed,
                meals[i].rating);

    fclose(fp);
}

void saveParcels(void) {
    FILE *fp = fopen("parcels.txt", "w");

    if (fp == NULL) {
        printf("Unable to save parcels.\n");
        return;
    }

    for (int i = 0; i < parcelCount; i++)
        fprintf(fp, "%d|%d|%s|%s|%d\n",
                parcels[i].parcelId, parcels[i].regid,
                parcels[i].courier, parcels[i].arrivalDate,
                parcels[i].status);

    fclose(fp);
}

void saveComplaints(void) {
    FILE *fp = fopen("complaints.txt", "w");

    if (fp == NULL) {
        printf("Unable to save complaints.\n");
        return;
    }

    for (int i = 0; i < complaintCount; i++)
        fprintf(fp, "%d|%d|%d|%s|%s|%d|%d\n",
                complaints[i].complaintId,
                complaints[i].studentId,
                complaints[i].roomNo,
                complaints[i].category,
                complaints[i].description,
                complaints[i].priority,
                complaints[i].status);

    fclose(fp);
}

void loadStudents(void) {
    FILE *fp = fopen("students.txt", "r");

    if (fp == NULL)
        return;

    while (studentCount < studentCapacity &&
           fscanf(fp, "%d|%19[^|]|%d",
                  &std[studentCount].id,
                  std[studentCount].name,
                  &std[studentCount].roomno) == 3)
        studentCount++;

    while (!feof(fp)) {
        struct Student s;

        if (fscanf(fp, "%d|%19[^|]|%d",
                   &s.id, s.name, &s.roomno) != 3)
            break;

        if (studentCount == studentCapacity &&
            !ensureCapacity((void **)&std, &studentCapacity,
                            sizeof(struct Student)))
            break;

        std[studentCount++] = s;
    }

    fclose(fp);
}

void loadMeals(void) {
    FILE *fp = fopen("meals.txt", "r");

    if (fp == NULL)
        return;

    struct Meal m;

    while (fscanf(fp, "%d|%29[^|]|%29[^|]|%d|%d|%f",
                  &m.id, m.name, m.type, &m.prepared,
                  &m.consumed, &m.rating) == 6) {
        if (mealCount == mealCapacity &&
            !ensureCapacity((void **)&meals, &mealCapacity,
                            sizeof(struct Meal)))
            break;

        meals[mealCount++] = m;
    }

    fclose(fp);
}

void loadParcels(void) {
    FILE *fp = fopen("parcels.txt", "r");

    if (fp == NULL)
        return;

    struct Parcel p;

    while (fscanf(fp, "%d|%d|%29[^|]|%14[^|]|%d",
                  &p.parcelId, &p.regid, p.courier,
                  p.arrivalDate, &p.status) == 5) {
        if (parcelCount == parcelCapacity &&
            !ensureCapacity((void **)&parcels, &parcelCapacity,
                            sizeof(struct Parcel)))
            break;

        parcels[parcelCount++] = p;
    }

    fclose(fp);
}

void loadComplaints(void) {
    FILE *fp = fopen("complaints.txt", "r");

    if (fp == NULL)
        return;

    struct Complaint c;

    while (fscanf(fp, "%d|%d|%d|%29[^|]|%99[^|]|%d|%d",
                  &c.complaintId, &c.studentId, &c.roomNo,
                  c.category, c.description,
                  &c.priority, &c.status) == 7) {
        if (complaintCount == complaintCapacity &&
            !ensureCapacity((void **)&complaints,
                            &complaintCapacity,
                            sizeof(struct Complaint)))
            break;

        complaints[complaintCount++] = c;
    }

    fclose(fp);
}

void addstudent(void) {
    int id;

    printf("Enter Student ID: ");
    scanf("%d", &id);

    if (id <= 0 || searchStudent(id) != NULL) {
        printf("Invalid or duplicate Student ID.\n");
        return;
    }

    if (studentCount == studentCapacity &&
        !ensureCapacity((void **)&std, &studentCapacity,
                        sizeof(struct Student)))
        return;

    std[studentCount].id = id;

    printf("Enter Student Name: ");
    scanf(" %19[^\n]", std[studentCount].name);

    printf("Enter Room Number: ");
    scanf("%d", &std[studentCount].roomno);

    if (std[studentCount].roomno <= 0) {
        printf("Invalid room number.\n");
        return;
    }

    studentCount++;
    saveStudents();

    printf("Student added successfully.\n");
}

void displayStudents(void) {
    if (studentCount == 0) {
        printf("No students registered.\n");
        return;
    }

    for (int i = 0; i < studentCount; i++)
        printf("\nID: %d | Name: %s | Room: %d\n",
               std[i].id, std[i].name, std[i].roomno);
}

void findStudent(void) {
    int id;

    printf("Enter Student ID: ");
    scanf("%d", &id);

    struct Student *p = searchStudent(id);

    if (p == NULL) {
        printf("Student not found.\n");
        return;
    }

    printf("ID: %d\nName: %s\nRoom: %d\n",
           p->id, p->name, p->roomno);
}

void deleteStudent(void) {
    int id, index = -1;

    printf("Enter Student ID to delete: ");
    scanf("%d", &id);

    for (int i = 0; i < parcelCount; i++) {
        if (parcels[i].regid == id) {
            printf("Cannot delete: student has parcel records.\n");
            return;
        }
    }

    for (int i = 0; i < complaintCount; i++) {
        if (complaints[i].studentId == id) {
            printf("Cannot delete: student has complaint records.\n");
            return;
        }
    }

    for (int i = 0; i < studentCount; i++) {
        if (std[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    for (int i = index; i < studentCount - 1; i++)
        std[i] = std[i + 1];

    studentCount--;
    saveStudents();

    printf("Student deleted successfully.\n");
}

void studentMenu(void) {
    int choice;

    do {
        printf("\n--- STUDENT MANAGEMENT ---\n");
        printf("1. Add Student\n2. Display Students\n");
        printf("3. Search Student\n4. Delete Student\n5. Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addstudent(); break;
            case 2: displayStudents(); break;
            case 3: findStudent(); break;
            case 4: deleteStudent(); break;
            case 5: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);
}

void addMeal(void) {
    int id;
    struct Meal m;

    printf("Enter Meal ID: ");
    scanf("%d", &id);

    if (id <= 0 || searchMeal(id) != NULL) {
        printf("Invalid or duplicate Meal ID.\n");
        return;
    }

    if (mealCount == mealCapacity &&
        !ensureCapacity((void **)&meals, &mealCapacity,
                        sizeof(struct Meal)))
        return;

    m.id = id;

    printf("Enter Food Item: ");
    scanf(" %29[^\n]", m.name);

    printf("Enter Meal Type: ");
    scanf(" %29[^\n]", m.type);

    printf("Enter Quantity Prepared: ");
    scanf("%d", &m.prepared);

    printf("Enter Quantity Consumed: ");
    scanf("%d", &m.consumed);

    if (m.prepared < 0 || m.consumed < 0 ||
        m.consumed > m.prepared) {
        printf("Invalid quantities.\n");
        return;
    }

    printf("Enter Rating (0-5): ");
    scanf("%f", &m.rating);

    if (m.rating < 0 || m.rating > 5) {
        printf("Invalid rating.\n");
        return;
    }

    meals[mealCount++] = m;
    saveMeals();

    printf("Meal record added successfully.\n");
}

void displayMeals(void) {
    if (mealCount == 0) {
        printf("No meal records found.\n");
        return;
    }

    for (int i = 0; i < mealCount; i++)
        printf("\nID: %d | Food: %s | Type: %s\n"
               "Prepared: %d | Consumed: %d | Wasted: %d\n"
               "Rating: %.1f/5\n",
               meals[i].id, meals[i].name, meals[i].type,
               meals[i].prepared, meals[i].consumed,
               meals[i].prepared - meals[i].consumed,
               meals[i].rating);
}

void findMeal(void) {
    int id;

    printf("Enter Meal ID: ");
    scanf("%d", &id);

    struct Meal *p = searchMeal(id);

    if (p == NULL) {
        printf("Meal not found.\n");
        return;
    }

    printf("Food: %s\nType: %s\nPrepared: %d\n"
           "Consumed: %d\nWasted: %d\nRating: %.1f\n",
           p->name, p->type, p->prepared, p->consumed,
           p->prepared - p->consumed, p->rating);
}

void mealMenu(void) {
    int choice;

    do {
        printf("\n--- MESS MANAGEMENT ---\n");
        printf("1. Add Meal\n2. Display Meals\n");
        printf("3. Search Meal\n4. Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addMeal(); break;
            case 2: displayMeals(); break;
            case 3: findMeal(); break;
            case 4: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 4);
}

void addParcel(void) {
    struct Parcel p;

    printf("Enter Parcel ID: ");
    scanf("%d", &p.parcelId);

    if (p.parcelId <= 0 || searchParcel(p.parcelId) != NULL) {
        printf("Invalid or duplicate Parcel ID.\n");
        return;
    }

    if (parcelCount == parcelCapacity &&
        !ensureCapacity((void **)&parcels, &parcelCapacity,
                        sizeof(struct Parcel)))
        return;

    printf("Enter Student ID: ");
    scanf("%d", &p.regid);

    if (searchStudent(p.regid) == NULL) {
        printf("Student not found. Register the student first.\n");
        return;
    }

    printf("Enter Courier Name: ");
    scanf(" %29[^\n]", p.courier);

    printf("Enter Arrival Date (DD-MM-YYYY): ");
    scanf(" %14s", p.arrivalDate);

    p.status = 0;
    parcels[parcelCount++] = p;

    saveParcels();
    printf("Parcel registered successfully.\n");
}

void displayParcels(void) {
    if (parcelCount == 0) {
        printf("No parcels registered.\n");
        return;
    }

    for (int i = 0; i < parcelCount; i++)
        printf("\nParcel ID: %d | Student ID: %d\n"
               "Courier: %s | Date: %s\nStatus: %s\n",
               parcels[i].parcelId, parcels[i].regid,
               parcels[i].courier, parcels[i].arrivalDate,
               parcels[i].status ? "Collected" : "Pending");
}

void findParcel(void) {
    int id;

    printf("Enter Parcel ID: ");
    scanf("%d", &id);

    struct Parcel *p = searchParcel(id);

    if (p == NULL) {
        printf("Parcel not found.\n");
        return;
    }

    printf("Parcel ID: %d\nStudent ID: %d\nCourier: %s\n"
           "Arrival Date: %s\nStatus: %s\n",
           p->parcelId, p->regid, p->courier,
           p->arrivalDate,
           p->status ? "Collected" : "Pending");
}

void updateParcel(void) {
    int id;

    printf("Enter Parcel ID to mark collected: ");
    scanf("%d", &id);

    struct Parcel *p = searchParcel(id);

    if (p == NULL) {
        printf("Parcel not found.\n");
        return;
    }

    p->status = 1;
    saveParcels();

    printf("Parcel marked as collected.\n");
}

void parcelMenu(void) {
    int choice;

    do {
        printf("\n--- PARCEL MANAGEMENT ---\n");
        printf("1. Register Parcel\n2. Display Parcels\n");
        printf("3. Search Parcel\n4. Mark Collected\n5. Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addParcel(); break;
            case 2: displayParcels(); break;
            case 3: findParcel(); break;
            case 4: updateParcel(); break;
            case 5: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);
}

void addComplaint(void) {
    struct Complaint c;

    printf("Enter Complaint ID: ");
    scanf("%d", &c.complaintId);

    if (c.complaintId <= 0 ||
        searchComplaint(c.complaintId) != NULL) {
        printf("Invalid or duplicate Complaint ID.\n");
        return;
    }

    if (complaintCount == complaintCapacity &&
        !ensureCapacity((void **)&complaints, &complaintCapacity,
                        sizeof(struct Complaint)))
        return;

    printf("Enter Student ID: ");
    scanf("%d", &c.studentId);

    struct Student *s = searchStudent(c.studentId);

    if (s == NULL) {
        printf("Student not found. Register the student first.\n");
        return;
    }

    c.roomNo = s->roomno;

    printf("Enter Category: ");
    scanf(" %29[^\n]", c.category);

    printf("Enter Description: ");
    scanf(" %99[^\n]", c.description);

    printf("Priority (1-Emergency, 2-High, 3-Medium, 4-Low): ");
    scanf("%d", &c.priority);

    if (c.priority < 1 || c.priority > 4) {
        printf("Invalid priority.\n");
        return;
    }

    c.status = 0;
    complaints[complaintCount++] = c;

    saveComplaints();
    printf("Complaint registered successfully.\n");
}

void displayComplaints(void) {
    if (complaintCount == 0) {
        printf("No complaints registered.\n");
        return;
    }

    for (int i = 0; i < complaintCount; i++) {
        struct Complaint *p = &complaints[i];

        printf("\nComplaint ID: %d | Student ID: %d\n"
               "Room: %d | Category: %s\n"
               "Description: %s\nPriority: %s\nStatus: %s\n",
               p->complaintId, p->studentId, p->roomNo,
               p->category, p->description,
               p->priority == 1 ? "Emergency" :
               p->priority == 2 ? "High" :
               p->priority == 3 ? "Medium" : "Low",
               p->status ? "Resolved" : "Pending");
    }
}

void findComplaint(void) {
    int id;

    printf("Enter Complaint ID: ");
    scanf("%d", &id);

    struct Complaint *p = searchComplaint(id);

    if (p == NULL) {
        printf("Complaint not found.\n");
        return;
    }

    printf("Complaint ID: %d\nStudent ID: %d\nRoom: %d\n"
           "Category: %s\nDescription: %s\nStatus: %s\n",
           p->complaintId, p->studentId, p->roomNo,
           p->category, p->description,
           p->status ? "Resolved" : "Pending");
}

void updateComplaint(void) {
    int id;

    printf("Enter Complaint ID to resolve: ");
    scanf("%d", &id);

    struct Complaint *p = searchComplaint(id);

    if (p == NULL) {
        printf("Complaint not found.\n");
        return;
    }

    p->status = 1;
    saveComplaints();

    printf("Complaint marked as resolved.\n");
}

void complaintMenu(void) {
    int choice;

    do {
        printf("\n--- COMPLAINT MANAGEMENT ---\n");
        printf("1. Register Complaint\n2. Display Complaints\n");
        printf("3. Search Complaint\n4. Resolve Complaint\n5. Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addComplaint(); break;
            case 2: displayComplaints(); break;
            case 3: findComplaint(); break;
            case 4: updateComplaint(); break;
            case 5: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);
}

void dashboard(void) {
    int pendingParcels = 0, pendingComplaints = 0;
    int prepared = 0, consumed = 0;
    float rating = 0;

    for (int i = 0; i < parcelCount; i++)
        if (!parcels[i].status)
            pendingParcels++;

    for (int i = 0; i < complaintCount; i++)
        if (!complaints[i].status)
            pendingComplaints++;

    for (int i = 0; i < mealCount; i++) {
        prepared += meals[i].prepared;
        consumed += meals[i].consumed;
        rating += meals[i].rating;
    }
    

    printf("\n========== HOSTELHUB DASHBOARD ==========\n");
    printf("Students: %d\n", studentCount);
    printf("Meal Records: %d\n", mealCount);
    printf("Food Prepared: %d\n", prepared);
    printf("Food Consumed: %d\n", consumed);
    printf("Food Wastage: %d\n", prepared - consumed);

    if (mealCount > 0)
        printf("Average Rating: %.2f/5\n", rating / mealCount);

    printf("Total Parcels: %d\n", parcelCount);
    printf("Pending Parcels: %d\n", pendingParcels);
    printf("Collected Parcels: %d\n", parcelCount - pendingParcels);

    printf("Total Complaints: %d\n", complaintCount);
    printf("Pending Complaints: %d\n", pendingComplaints);
    printf("Resolved Complaints: %d\n",
           complaintCount - pendingComplaints);
}

int main(void) {
    int choice;

    std = malloc(MAX * sizeof(struct Student));
    meals = malloc(MAX * sizeof(struct Meal));
    parcels = malloc(MAX * sizeof(struct Parcel));
    complaints = malloc(MAX * sizeof(struct Complaint));

    if (std == NULL || meals == NULL ||
        parcels == NULL || complaints == NULL) {
        printf("Memory allocation failed.\n");
        free(std);
        free(meals);
        free(parcels);
        free(complaints);
        return 1;
    }

    loadStudents();
    loadMeals();
    loadParcels();
    loadComplaints();

    do {
    		printf("\n");
	printf("Sanusha Nair 25BCE7973\n");
	printf("\n");
        printf("\n========== HOSTELHUB ==========\n");
        printf("1. Student Management\n");
        printf("2. Mess Management\n");
        printf("3. Parcel Management\n");
        printf("4. Complaint Management\n");
        printf("5. Dashboard\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: studentMenu(); break;
            case 2: mealMenu(); break;
            case 3: parcelMenu(); break;
            case 4: complaintMenu(); break;
            case 5: dashboard(); break;
            case 6:
                saveStudents();
                saveMeals();
                saveParcels();
                saveComplaints();
                printf("Records saved. Exiting HostelHub.\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 6);

    free(std);
    free(meals);
    free(parcels);
    free(complaints);

    return 0;
}

