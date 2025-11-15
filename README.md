# Spam-Mail-Filter
This project is a comprehensive Spam Mail Filter built using a robust 3-tier architecture. It effectively combines a user-friendly web interface, a Python API, and a high-performance C program to create a complete and practical application.]

The first tier is the frontend, a dynamic webpage (index.html). This interface allows a user to submit email details (sender, body file, attachments) to be checked, and it also provides forms to manually add senders to the whitelist or blacklist. All user interaction is handled by JavaScript, which uses fetch to send data to the backend.

The second tier is a Python Flask API (app.py), which acts as the crucial middle-layer. It receives JSON data from the frontend's JavaScript. Instead of performing the logic itself, it translates these web requests into command-line arguments and uses the subprocess module to execute the compiled C program. It then captures the C program's console output (e.g., "FINAL_RESULT:BLOCKED") and sends it back to the frontend as a JSON response.

The third tier is the C Core Engine, which contains all the filter logic. The processEmail function in main.c performs a three-step analysis:

Sender Reputation: It uses a hash table (hashmap.c) to instantly check if the sender is on a persistent blacklist or whitelist (loaded from sender_database.csv).

Content Analysis: It reads the email body and uses a second hash table to check for spam keywords (loaded from keywords.txt), flagging the email if its score exceeds a threshold.

Attachment Threat: It uses a stack (stack.c) to process attachments, checking each filename for dangerous extensions like .exe or .bat.

The project demonstrates a strong use of data structures, including hash tables for fast lookups, a stack for attachment processing, a queue for the C program's interactive mode, and a graph to model sender relationships.
