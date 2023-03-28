#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <fcntl.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

int c,new_socket;
std::vector<uchar> buffer;
std::vector<uchar> arrak;
int i=0,k=0;
int bbytee;
Mat image;
int bytes=0;

int main() {
    int sock,listener;
    std::vector<uchar> buf;
    std::vector<int> params(2);
    params[0] = cv::IMWRITE_PNG_COMPRESSION;
    params[1] = 10;
    struct sockaddr_in addr,client;

    sock = socket(AF_INET, SOCK_STREAM, 0); // создание сокета
    if(sock < 0) {
        perror("socket");
        exit(1);
    }
    fcntl(sock, F_SETFL, O_NONBLOCK); // установка неблокирующего режима

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // подключение к любому IP-адресу

    VideoCapture cap(2); // захват видеокамеры

        if(!cap.isOpened()) { // проверка условий подключения
            cout<< "Could not open the camera" <<  endl;
            close(sock);
            return -1;
        }
    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) { // связывание сокетов
        perror("[server] binding failed!");
        exit(2);
    }

    Mat frame = Mat::zeros(1080, 1920, CV_8UC3); // создание матрицы изображения
    int imgSize = frame.cols*frame.rows*3; // узнаем размер изображения на три канала

    listen(sock, 1); // прослушивание подключений

    c = sizeof(struct sockaddr_in); // размер структуры

    while (true) {

        puts("Waiting for incoming connections...");
        new_socket = accept(sock, (struct sockaddr *)&client, (socklen_t*)&c);// прием сокета
        cout << "New_socket: " << new_socket << endl;
        cap >> frame;//чтение с cap в frame
        imencode(".jpg", frame, buffer,params); //перекодирование в буфер buffer

               if(frame.empty()) {
                   cerr<<"[client] VideoCapture(0) error!"<<endl;
               }

               cout<< ++i << ":"<< frame.isContinuous()<<"," <<frame.size()<<endl;

        if(new_socket == -1) {//если подключения клиента нет, то...
            puts("Connection not accepted");
            cout << "new_socket = " << new_socket << "\n";
        } else {//если подключился клиент, то ..
        	 int size = buffer.size()*sizeof buffer[0];//определяем размер буфера
        	 send(new_socket, &size, sizeof(size), 0);

        	 cout << "Razmer" << size << endl;
            cout << "Transmission started" << endl;

            while(true){
            	 if ((bytes=send(new_socket,buffer.data(), size, 0))<0){
            		 cout << "BYTES: "<<bytes<<endl;
            		 break;
            		 // передаем размер буфера клиенту
            	 }

            }


            //break;
        }
    }
    cout << "END" << endl;
    close(sock); // закрытие сокета
    return 0;
}

