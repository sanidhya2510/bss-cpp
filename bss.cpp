#include<bits/stdc++.h>
using namespace std;

class Process;


class Message{
    public:
    int message_id;
    Process* sender;
    vector<int> timestamp;

    Message(string message, Process* sender);

};

class Process {
    public:
    int pid;
    int current_task;
    vector<int>vector_clock;
    vector<string>operation;
    vector<Message*> delayed_message_queue;
    map<Message*, vector<int>> send_buffer;
    vector<string>final_operations;
    Process(string name, int num);
};

Message::Message(string message, Process* sender) {
    this->message_id = stoi(message.substr(1));
    this->sender = sender;
    this->sender->vector_clock[sender->pid]++;
    this->timestamp = sender->vector_clock;
}

Process::Process(string name, int num) {
    pid = stoi(name.substr(1))-1;
    current_task = 0;
    vector_clock.resize(num, 0);
}

string vectorToString(vector<int> vec){
    string result = "[";
    for(int i = 0; i < vec.size(); i++){
        result.append(to_string(vec[i]));
        if(i != vec.size()-1){
            result.append(" ");
        }
    }
    result.append("]");
    return result;
}

void parseInput(map<string, Process*> & process_list, int num){
    string line;
    Process* current_process = nullptr;
    while(getline(cin, line)){
        if(line.find("begin process") != string::npos){
            stringstream ss(line);
            string temp, processName;
            ss >> temp>> temp>> processName;

            current_process = new Process(processName, num);
            process_list[processName] = current_process;
        }

        else if (line.find("end process") != string::npos) {
            current_process->operation.push_back("end");
            current_process = nullptr;
        }

        else if(current_process != nullptr){
            current_process->operation.push_back(line);
    }
}
}

void BSS(map<string, Process*> &process_list, map<string, Message*> &message_list, set<string> &sent_messages){
    int break_counter = 0;
    while(break_counter < process_list.size()){
        for(auto it = process_list.begin(); it!=process_list.end(); it++){
            Process* current_process = it->second;
            // cout<<current_process->pid<<endl;
            if(current_process->current_task == current_process->operation.size()){
                    break;
                }
            while(true){
                string operation = current_process->operation[current_process->current_task];
                // cout<<operation<<endl;
                if(operation.find("send") != string::npos){
                    stringstream ss(operation);
                    string temp, message;
                    ss >> temp >> message;
                    Message* new_message = new Message(message, current_process);
                    message_list[message] = new_message;
                    current_process->send_buffer[new_message] = new_message->timestamp;
                    sent_messages.insert(message);
                    // cout<<"Message sent\n";
                    current_process->current_task++;
                    current_process->final_operations.push_back(operation.append(" ").append(vectorToString(new_message->timestamp)));
                }

                else if(operation.find("recv_B") != string::npos){
                    stringstream ss(operation);
                    string temp, process, message;
                    ss >> temp >> process >> message;
                    // cout<<message<<endl;
                    // for(auto it: sent_messages){
                    //     cout<<it<<endl;
                    // }
                    if(sent_messages.find(message) != sent_messages.end()){
                        Message* received_message = message_list[message];
                        bool is_delivered = true;
                        // cout<<vectorToString(received_message->timestamp)<<endl;
                        for(int i = 0; i < current_process->vector_clock.size(); i++){
                            if(received_message->timestamp[i] > current_process->vector_clock[i] && i != received_message->sender->pid){
                                is_delivered = false;
                                break;
                            }
                            else if(i == received_message->sender->pid && received_message->timestamp[i] != current_process->vector_clock[i]+1){
                                is_delivered = false;
                                break;
                            }
                            else continue;
                        }

                        current_process->final_operations.push_back(operation + " " + (vectorToString(current_process->vector_clock)));
                        if(is_delivered){
                            // cout<<"Message delivered\n";
                            for(int i = 0; i < current_process->vector_clock.size(); i++){
                                current_process->vector_clock[i] = max(current_process->vector_clock[i], received_message->timestamp[i]);
                            }
                            string new_receive_string = "recv_A ";
                            current_process->final_operations.push_back(new_receive_string.append(operation.substr(7)) + (" ") + vectorToString(current_process->vector_clock));

                            while(true){
                                int initial_size = current_process->delayed_message_queue.size();
                                for(auto itr = current_process->delayed_message_queue.begin(); itr != current_process->delayed_message_queue.end(); itr++){
                                    bool is_popped = true;
                                    for(int i = 0; i < current_process->vector_clock.size(); i++){
                                        if((*itr)->timestamp[i] > current_process->vector_clock[i] && i != (*itr)->sender->pid){
                                            is_popped = false;
                                            break;
                                        }
                                        else if(i == (*itr)->sender->pid && (*itr)->timestamp[i] != current_process->vector_clock[i]+1){
                                            is_popped = false;
                                            break;
                                        }
                                        else {
                                            continue;
                                        }
                                    }

                                    if(is_popped){
                                        // cout<<"Message delivered\n";
                                        // current_process->final_operations.push_back("recv_B " + to_string((*itr)->message_id) + " " + it->first + " " + vectorToString(current_process->vector_clock));
                                        for(int i = 0; i < current_process->vector_clock.size(); i++){
                                            current_process->vector_clock[i] = max(current_process->vector_clock[i], (*itr)->timestamp[i]);
                                        }
                                        string new_receive_string = "recv_A ";
                                        current_process->final_operations.push_back(new_receive_string.append("m" + to_string((*itr)->message_id) + " " + it->first) + (" ") + vectorToString(current_process->vector_clock));
                                        current_process->delayed_message_queue.erase(itr);
                                        break;
                                    }

                                    else{
                                        continue;
                                    }
                                }

                                if(initial_size == current_process->delayed_message_queue.size()){
                                    break;
                                }
                            }
                        }

                        else{
                            // cout<<"Message not delivered\n";
                            current_process->delayed_message_queue.push_back(received_message);
                        }
                        current_process->current_task++;                    
                    }

                    else {
                        // cout<<"Message not found\n";
                        break;
                    }
                }

                else if(operation.find("end") != string::npos){
                    break_counter++;
                    break;
                }
            }
        }
    }
}   

int main(){
    int num = 4;
    map<string, Process*> process_list;
    map<string, Message*> message_list;
    set<string> sent_messages;
    parseInput(process_list, num);
    // cout<<"Parsed input\n";
    BSS(process_list, message_list, sent_messages);
    // cout<<"BSS done\n";
    for(auto it = process_list.begin(); it!=process_list.end(); it++){
        Process* current_process = it->second;
        cout << "begin process " << it->first << ":\n";
        for(int i = 0; i < current_process->final_operations.size(); i++){
            cout << current_process->final_operations[i] << endl;
        }
        cout << "end process " << it->first << endl;
    }
    return 0;
}
