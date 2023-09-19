#include <stdio.h>
#include <unistd.h>
#include <tebot.h>
#include <creqhttp.h>

#define TOKEN        ""
#define IS_DOWNLOADING_FILE                0

static tebot_handler_t *h;

static void input_handle (creqhttp_epoll_event *v) {
       	http_req *htr = creqhttp_parse_request (v->data.data, v->data.len);
	if (htr) {
		tebot_result_updated_t *t = tebot_get_data_from_webhook (h, htr->post_data);
		if (!t) {
			v->flags[IS_DOWNLOADING_FILE] = 1;
			v->is_disconnect = 0;
			return;
		}

		if (t->update[0]->message) {
			if (t->update[0]->message->text) {
				printf ("%s: %s\n",
						t->update[0]->message->from->username,
						t->update[0]->message->text
				       );
	
				struct tebot_send_message_t m;
				memset (&m, 0, sizeof (struct tebot_send_message_t));

				m.chat_id = t->update[0]->message->from->id;
				m.text = t->update[0]->message->text;

				tebot_method_send_message (h, &m);
			} 
		}

		v->is_disconnect = 1;

		char *ans = "HTTP/1.1 200 OK\r\n\r\n";
		memcpy (v->data.ans_data, ans, strlen (ans) + 1);
		v->data.ans_len = strlen (ans);
		v->data.is_answer = 1;

	} else if (v->flags[IS_DOWNLOADING_FILE]) {

		tebot_result_updated_t *t = tebot_get_data_from_webhook (h, v->data.data);

		v->flags[IS_DOWNLOADING_FILE] = 0;

		if (t->update[0]->message->document) {
			printf ("%s: %s\n",
					t->update[0]->message->from->username,
					t->update[0]->message->document->file_name
			       );
		}

		if (t->update[0]->message->document) {
			char *file_id = t->update[0]->message->document->file_id;
			char *outfile = t->update[0]->message->document->file_name;

			tebot_method_get_file (h, file_id, outfile);
		}
		v->is_disconnect = 1;

		char *ans = "HTTP/1.1 200 OK\r\n\r\n";
		memcpy (v->data.ans_data, ans, strlen (ans) + 1);
		v->data.ans_len = strlen (ans);
		v->data.is_answer = 1;
	}
}

int main (int argc, char **argv) {
	struct tebot_setup_webhook setup = {
		.port = 8080,
		.is_ssl = 1,
		.msg_handle = input_handle,
		.route = "https://your_site:8443/hook",
		.cert_file = "lets_encrypt/fullchain.pem",
		.private_key_file = "lets_encrypt/privkey.pem"
			
	};

	h = tebot_init (TOKEN, TEBOT_DEBUG_NOT_SHOW, NULL);

	tebot_set_webhook (h, &setup);

	while (1) {
		sleep (1);
	}
}
