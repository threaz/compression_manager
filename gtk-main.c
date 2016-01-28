//#include <stdio.h>
//#include <string.h>
//#include <locale.h>
//#include <gtk/gtk.h>

//#include "option.h"
//#include "archive.h"

//#define MAX_COMMAND 2000
//#define MAX_ARGS 100

//static struct buttons {
//   char *descr, *output;
//   int posX,lenX,posY,lenY;
//} tab[] = {
//{"Utwórz archiwum","-c ",0,3,0,1}, {"Dodaj do archiwum","-a ",0,3,2,1},
//{"Wypakuj z archiwum","-e ",0,3,4,1}, {"Wyjdź z programu", "q",0,3,5,1}
//                                      };

//struct MyDialog
//{
//   char command[MAX_COMMAND];
//   GtkEntryBuffer *entry;
//};

//typedef struct MyDialog* dial;

//void third_level(GtkWidget *widget, gpointer* gathered_text)
//{
//   dial data1 = (dial)gathered_text;

//   strncat(data1->command, gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(data1->entry)), MAX_COMMAND);
//   strncat(data1->command, " ", MAX_COMMAND);

//   fputs(data1->command, stderr);

//   char *ptr_arr[MAX_ARGS];
//   int cnt = 0;
//   char *pch = strtok(data1->command, " ");
//   while(pch != NULL)
//   {
//      ptr_arr[cnt++] = pch;
//      pch = strtok(NULL, " ");
//   }

//   GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//   gtk_container_set_border_width(GTK_CONTAINER (window), 30);
//   gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);

//   archive_data data; // miejsce na parametry
//   archive arch;      // nowe archiwum
//   action_t act;      // akcja, która zostanie wykonana na archiwum


//   // inicjalizuje początkowe opcje
//   option_t options = init_option();

//   // "wyłuskuje" opcje z argv i zapisuje dane w options
//   if(extract_options(cnt, ptr_arr, &options) != 0)
//      return -1;

//   data = get_archive_data(options);
//   act  = get_archive_action(options);
//   arch = NULL;

//   // wykonaj act na arch używając danych z data
//   make_action_on_archive(arch, &act, &data);

//   gtk_widget_show_all(window);
//}

//void second_level(GtkWidget *widget, gpointer* gathered_text)
//{
//   dial data = (dial)gathered_text;

//   strncat(data->command, gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(data->entry)), MAX_COMMAND);
//   strncat(data->command, " ", MAX_COMMAND);

//   fputs(data->command, stderr);

//   GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//   g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(gtk_window_close), NULL);
//   gtk_container_set_border_width(GTK_CONTAINER (window), 30);
//   gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);

//   GtkWidget *grid = gtk_grid_new();
//   gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
//   gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
//   gtk_grid_set_column_spacing(GTK_GRID(grid), 3);
//   gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

//   gtk_container_add(GTK_CONTAINER(window), grid);

//   GtkEntryBuffer *entry_buffer = gtk_entry_buffer_new("", 0);
//   GtkWidget *text_field = gtk_entry_new_with_buffer(GTK_ENTRY_BUFFER(entry_buffer));
//   gtk_entry_set_max_length(GTK_ENTRY(text_field), MAX_COMMAND);
//   gtk_entry_set_alignment(GTK_ENTRY(text_field), 1);
//   gtk_grid_attach(GTK_GRID(grid), text_field, 0, 0, 4, 1);

//   //free(data->entry);
//   data->entry = entry_buffer;
//   GtkWidget *button = gtk_button_new_with_label("Potwierdź");
//   g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(third_level), (gpointer *)data);
//   gtk_grid_attach(GTK_GRID(grid), button, 0,2,4,1);

//   gtk_widget_show_all(window);
//}

//void first_level(GtkWidget *widget, gpointer gathered_text)
//{
//   char command[MAX_COMMAND] = {"kompresuj "};
//   strcat(command, gathered_text);
//   dial dat = (dial)malloc(sizeof(struct MyDialog));
//   fputs(command, stderr);

//   GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//   g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(gtk_window_close), NULL);
//   gtk_container_set_border_width(GTK_CONTAINER (window), 30);

//   GtkWidget *grid = gtk_grid_new();
//   gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
//   gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
//   gtk_grid_set_column_spacing(GTK_GRID(grid), 3);
//   gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

//   gtk_container_add(GTK_CONTAINER(window), grid);

//   GtkEntryBuffer *entry_buffer = gtk_entry_buffer_new("", 0);
//   GtkWidget *text_field = gtk_entry_new_with_buffer(GTK_ENTRY_BUFFER(entry_buffer));
//   gtk_entry_set_max_length(GTK_ENTRY(text_field), MAX_COMMAND);
//   gtk_entry_set_alignment(GTK_ENTRY(text_field), 1);
//   gtk_grid_attach(GTK_GRID(grid), text_field, 0, 0, 4, 1);

//   strncpy(dat->command, command, MAX_COMMAND);
//   dat->entry = entry_buffer;

//   GtkWidget *button = gtk_button_new_with_label("Potwierdź");
//   g_object_set_data(G_OBJECT(button), "entry1", command);
//   g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(second_level), (gpointer *)dat);
//   gtk_grid_attach(GTK_GRID(grid), button, 0,2,4,1);

//   gtk_widget_show_all(window);
//}

//static GtkWidget *main_window;

//int main(int argc, char *argv[])
//{
//   GtkWidget *button;

//   gtk_init(&argc, &argv);
//   setlocale(LC_ALL, "pl_PL.UTF-8");

//   main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//   gtk_window_set_title(GTK_WINDOW(main_window),"kompresuj");
//   gtk_window_set_position(GTK_WINDOW(main_window),GTK_WIN_POS_CENTER);
//   g_signal_connect(G_OBJECT(main_window), "destroy",G_CALLBACK(gtk_main_quit), NULL);
//   gtk_container_set_border_width(GTK_CONTAINER(main_window), 10);

//   GtkWidget *grid = gtk_grid_new();
//   gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
//   gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
//   gtk_grid_set_column_spacing(GTK_GRID(grid), 3);
//   gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
//   gtk_container_add(GTK_CONTAINER(main_window), grid);

//   unsigned int i;
//   for(i = 0; i < sizeof(tab)/sizeof(struct buttons); i++) {
//      button = gtk_button_new_with_label(tab[i].descr);
//      g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(first_level),
//                       (gpointer) tab[i].output);
//      gtk_grid_attach(GTK_GRID(grid), button, tab[i].posX, tab[i].posY, tab[i].lenX,
//                      tab[i].lenY);
//   }

//   // podłącz wyjście do ostatniego przycisku
//   g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(gtk_main_quit), (gpointer)tab[i-1].output);

//   gtk_widget_show_all(main_window);
//   gtk_main();

//   return 0;
//}
