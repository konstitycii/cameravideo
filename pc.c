#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int recv_size;
Mat frame;
std::vector<uchar> buffer;
int size = 0;

int main() {
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);// создание нового сокета
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_addr.s_addr = inet_addr("192.168.3.11");// IP камеры

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) { // подключение к серверу
        perror("connect");
        exit(2);
    }

    if ((recv_size = recv(sock, (char*)&size, sizeof(uint32_t), 0)) != sizeof(uint32_t)) { //прием размера буфера
        perror("recv");
        exit(3);
    }

    cout << "SIZE FRAME: " << size << endl;


    VideoWriter outputVideo;
    Size S = Size((int)1920, (int)1080);//размер изображения
    outputVideo.open("receive.avi", VideoWriter::fourcc('H', '2', '6', '4'), 25, S, true);//открытие файла receive.avi для записи

    buffer.resize(size);//изменение буфера клиента под размер буфера сервера
    int bytes_received = 0;
while (bytes_received < size) {
    int chunk_size = min(size - bytes_received, 4096); // принимать до 4096 байт за раз
    int recv_size = recv(sock, &buffer[bytes_received], chunk_size, 0);//получение данных от сервера
    if (recv_size <= 0) {//если принимаемое, меньше либо равно 0, то выход из цикла
        perror("recv");
        exit(3);
    }
    bytes_received += recv_size;//увеличиваем размер количество принятого
}
    cout << "IDET DO IMDECODE " << bytes_received << endl;

    frame = imdecode(buffer, IMREAD_COLOR);//конвертирование данных в формат Mat
    
    outputVideo << frame;//запись буфера в файл receive.avi с указанными параметрами
    close(sock);
    outputVideo.release();//освобождение outputVideo

    return 0;
}

