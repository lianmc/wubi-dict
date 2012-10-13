/*
 * Copyright (C) 2009 llpk00.
 *
 * This is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <gtk/gtk.h>
#define VERSION "0.1"

#ifndef DEBUG
#define DATA_DIR "/usr/share/wubi-dict/data/"
#define ICON_DIR "/usr/share/pixmaps/"
#else
#define DATA_DIR "./data/" 
#define ICON_DIR "./data/"
#endif

enum {
	HANZI = 0,
	BIANMA,
	COLUMNS
};

typedef struct {
	GtkWidget *treeview;
	GtkWidget *radio_86;
	GtkWidget *entry;
} WubiList;

GPid pid;
GtkWidget *dialog;
WubiList *wubilist;

void on_zigen_clicked(GtkButton *info, GtkWindow *parent);
void about_activated(GtkButton *button, gpointer data);
void dialog_destroy(GtkDialog *dialog, gint response_id, gpointer data);
void search_begin();
gboolean read_output (GIOChannel *channel, GIOCondition condition, gpointer store);

int main(int argc, char *argv[])
{
	GtkWidget *window, *scrolled_win;
	GtkWidget *search, *radio_98, *zigen, *about, *close;
	GtkWidget *hbox1, *hbox2, *vbox;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	gtk_init(&argc, &argv);
	wubilist = g_slice_new0(WubiList);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_icon_from_file(GTK_WINDOW(window), 
			ICON_DIR"wubi-dict.png", NULL);
	gtk_window_set_title(GTK_WINDOW(window), "五笔编码查询");
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);
	gtk_widget_set_size_request(window, 250, 275);
	g_signal_connect(G_OBJECT(window), "destroy", 
			 G_CALLBACK(gtk_main_quit), NULL);

	wubilist->entry = gtk_entry_new();
	search = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(search), gtk_image_new_from_stock(
				GTK_STOCK_FIND, GTK_ICON_SIZE_SMALL_TOOLBAR));
	gtk_button_set_relief(GTK_BUTTON(search), GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(search, "Begin Query");
	g_signal_connect(G_OBJECT(search), "clicked", 
			 G_CALLBACK(search_begin), NULL);
	g_signal_connect(GTK_CELL_EDITABLE(wubilist->entry), "activate",  // no editing-done signal
			 G_CALLBACK(search_begin), NULL);

	hbox1 = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start_defaults(GTK_BOX(hbox1), wubilist->entry);
	gtk_box_pack_end(GTK_BOX(hbox1), search, FALSE, FALSE, 0);
			
	wubilist->treeview = gtk_tree_view_new();

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(
			"汉字", renderer, "text", HANZI, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(wubilist->treeview), column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(
			"编码", renderer, "text", BIANMA, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(wubilist->treeview), column);

	scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_win), wubilist->treeview);

	radio_98 = gtk_radio_button_new_with_label(NULL, "98版");
	wubilist->radio_86 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_98), "86版");

	zigen = gtk_toggle_button_new();
	gtk_button_set_image(GTK_BUTTON(zigen), gtk_image_new_from_stock(
				GTK_STOCK_INFO, GTK_ICON_SIZE_SMALL_TOOLBAR));
	gtk_button_set_relief(GTK_BUTTON(zigen), GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(zigen, "五笔字根图");
	g_signal_connect(G_OBJECT(zigen), "clicked",
			 G_CALLBACK(on_zigen_clicked), (gpointer)window);

	about = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(about), gtk_image_new_from_stock(
				GTK_STOCK_ABOUT, GTK_ICON_SIZE_SMALL_TOOLBAR));
	gtk_button_set_relief(GTK_BUTTON(about), GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(about, "About");
	g_signal_connect(G_OBJECT(about), "clicked", 
			 G_CALLBACK(about_activated), NULL);

	close = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(close), gtk_image_new_from_stock(
				GTK_STOCK_QUIT, GTK_ICON_SIZE_SMALL_TOOLBAR));
	gtk_button_set_relief(GTK_BUTTON(close), GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text(close, "Quit");
	g_signal_connect(G_OBJECT(close), "clicked", 
			 G_CALLBACK(gtk_main_quit), NULL);

	hbox2 = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start_defaults(GTK_BOX(hbox2), wubilist->radio_86);
	gtk_box_pack_start_defaults(GTK_BOX(hbox2), radio_98);
	gtk_box_pack_end(GTK_BOX(hbox2), close, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(hbox2), about, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(hbox2), zigen, FALSE, FALSE, 0);
	
	vbox = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_win, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);
	
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}

void search_begin()
{
	GtkListStore *store;
	gint argc, fout;
	const gchar *word;
	gchar *command;
	gchar **argv;
	GIOChannel *channel = NULL;

	store = gtk_list_store_new(COLUMNS, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (wubilist->treeview), GTK_TREE_MODEL (store));
  	g_object_unref (store);
	
	word = gtk_entry_get_text(GTK_ENTRY(wubilist->entry));

	if (g_utf8_strlen(word, -1) == 0)
		return ;
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wubilist->radio_86)))
	{
		command = g_strdup_printf(
				DATA_DIR"wb-dict.sh %s "DATA_DIR"/wb86.txt", word);
	}
	else
		command = g_strdup_printf(DATA_DIR"wb-dict.sh %s", word);

#ifdef DEBUG
	g_print("%s", command);
#endif
	
	g_shell_parse_argv(command, &argc, &argv, NULL);
	g_spawn_async_with_pipes(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL,
			&pid, NULL, &fout, NULL, NULL);
	g_strfreev(argv);
	g_free(command);
	
	channel = g_io_channel_unix_new(fout);
	g_io_add_watch(channel, G_IO_IN | G_IO_ERR | G_IO_HUP, read_output, store);
	g_io_channel_unref(channel);
}

gboolean read_output (GIOChannel *channel, GIOCondition condition, gpointer store)
{
	GError *error = NULL;
	GIOStatus status;
	gchar *line;
	gsize len, term;
	GString *hanzi, *bianma;
	GtkTreeIter iter;
	GString *temp;
	gssize offset;
	
  	/* Read the current line of data from the IO channel. */
	status = g_io_channel_read_line (channel, &line, &len, &term, &error);
	
	/* If some type of error has occurred, handle it. */
	if (status != G_IO_STATUS_NORMAL || line == NULL || error != NULL) 
	{
		if (error) 
		{
			g_warning ("Error reading IO channel: %s", error->message);
			g_error_free (error);
		}
    
    	if (channel != NULL)
      		g_io_channel_shutdown (channel, TRUE, NULL);
    	channel = NULL;
	return FALSE;
	}

	// display 
	offset = g_strstr_len(line, len, " ") - line; // offset of hanzi and bianma
      	hanzi = g_string_new_len(line, offset);
	temp = g_string_new_len(line, term); // erase linefeed
	bianma = g_string_ascii_up(g_string_erase(temp, 0, offset + 1));

	gtk_list_store_append (GTK_LIST_STORE(store), &iter);
      	gtk_list_store_set (GTK_LIST_STORE(store), &iter, HANZI, hanzi->str, BIANMA, bianma->str,-1); 
      	
	return TRUE;
}

void on_zigen_clicked(GtkButton *zigen, GtkWindow *parent)
{
	GtkWidget *image;

	if (dialog == NULL) {
		dialog = gtk_dialog_new_with_buttons("五笔字根图", parent,
				GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
		gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
		gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
		gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_NONE);
		gtk_window_set_icon_from_file(GTK_WINDOW(dialog), 
			ICON_DIR"wubi-dict.png", NULL);
		gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wubilist->radio_86)))
			image = gtk_image_new_from_file(DATA_DIR"zigen86.png");
		else
			image = gtk_image_new_from_file(DATA_DIR"zigen98.png");
	
		gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), image);
		g_signal_connect(G_OBJECT(dialog), "response",
			 G_CALLBACK(dialog_destroy), NULL);
		gtk_widget_show_all(dialog);

	} else {
		gtk_widget_destroy(dialog);
		gtk_widget_destroyed(dialog, &dialog);	
	}
}

// to insure only one copy of dialog
void dialog_destroy(GtkDialog *dialog2, gint response_id, gpointer data)
{
	gtk_widget_destroy(dialog);
	gtk_widget_destroyed(dialog, &dialog);
}

void about_activated(GtkButton *button, gpointer data)
{
	GtkWidget * about_dlg;
	const gchar *authors[] = {
		"llpk00 <lianmingchang@gmail.com>",
        	NULL
   	};

    	about_dlg = gtk_about_dialog_new ();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dlg), "WubiDict");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dlg), VERSION);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dlg), 
			"(C) 2009 Mingchang Lian");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dlg),
			"五笔编码查询");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dlg),
			"http://llpk00.is-programmer.com");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dlg), authors);
	gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about_dlg), 
			"源代码 GPL\n拼音和五笔词库来自FCITX\n五笔字根图来自金山打字截图\n程序图标来自ibus-table-wubi");

    	gtk_dialog_run(GTK_DIALOG(about_dlg));
    	gtk_widget_destroy(about_dlg);
}

