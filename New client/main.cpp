#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <ws2tcpip.h>
using namespace std;

// Підключаємо необхідні бібліотеки для роботи з мережевими сокетами у Windows
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512 // Розмір буфера для отримання даних
#define DEFAULT_PORT "27015" // Порт, який використовуватиме клієнт для підключення

#define PAUSE 1000 // Затримка для зручності відображення повідомлень

int main(int argc, char** argv)
{
    // Встановлення локалізації та зміна заголовка вікна консолі
    setlocale(0, "Ukrainian");
    system("title CLIENT SIDE");
    cout << "Процес клiєнта запущено!\n";
    Sleep(PAUSE);

    // Ініціалізація бібліотеки Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup не вдалося з помилкою: " << iResult << "\n";
        return 11;
    }
    cout << "Підключення до Winsock.dll пройшло успішно!\n";
    Sleep(PAUSE);

    // Налаштування структури addrinfo для отримання адреси сервера
    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;   // Дозволяє використовувати IPv4 або IPv6
    hints.ai_socktype = SOCK_STREAM; // Використання потоку даних (TCP)
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP

    const char* ip = "localhost"; // Адреса сервера (локальний комп'ютер)
    addrinfo* result = NULL;
    iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo не вдалося з помилкою: " << iResult << "\n";
        WSACleanup();
        return 12;
    }
    cout << "Отримання адреси та порту клієнта пройшло успішно!\n";
    Sleep(PAUSE);

    // Створення сокета та спроба підключення до сервера
    SOCKET ConnectSocket = INVALID_SOCKET;
    for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            cout << "Помилка створення сокета: " << WSAGetLastError() << "\n";
            WSACleanup();
            return 13;
        }

        // Підключення до сервера
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        cout << "Створення сокета на клієнті пройшло успішно!\n";
        Sleep(PAUSE);
        break;
    }
    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Не вдалося підключитися до сервера. Перевірте, чи запущено процес сервера!\n";
        WSACleanup();
        return 14;
    }
    cout << "Підключення до сервера пройшло успішно!\n";
    Sleep(PAUSE);

    // Відправлення повідомлення серверу
    const char* message = "hello from client!";
    iResult = send(ConnectSocket, message, (int)strlen(message), 0);
    if (iResult == SOCKET_ERROR) {
        cout << "Помилка при відправленні: " << WSAGetLastError() << "\n";
        closesocket(ConnectSocket);
        WSACleanup();
        return 15;
    }
    cout << "Дані успішно відправлено серверу: " << message << "\n";
    cout << "Байтів відправлено: " << iResult << "\n";
    Sleep(PAUSE);

    // Завершення відправлення даних
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        cout << "Помилка при завершенні з'єднання: " << WSAGetLastError() << "\n";
        closesocket(ConnectSocket);
        WSACleanup();
        return 16;
    }
    cout << "Клієнт ініціює закриття з'єднання.\n";

    // Отримання відповіді від сервера
    char answer[DEFAULT_BUFLEN];
    do {
        iResult = recv(ConnectSocket, answer, DEFAULT_BUFLEN, 0);
        if (iResult > 0) {
            answer[iResult] = '\0'; // Додаємо термінальний нуль, щоб правильно відобразити рядок
            cout << "Сервер надіслав відповідь: " << answer << "\n";
            cout << "Байтів отримано: " << iResult << "\n";
        } else if (iResult == 0) {
            cout << "З'єднання з сервером закрито.\n";
        } else {
            cout << "Помилка отримання даних: " << WSAGetLastError() << "\n";
        }
    } while (iResult > 0);

    // Закриття сокета і звільнення ресурсів
    closesocket(ConnectSocket);
    WSACleanup();
    cout << "Процес клієнта завершує свою роботу!\n";
    return 0;
}
