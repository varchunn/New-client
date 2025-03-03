// якщо у вас мак, тоді буде щось накшталт: https://gist.github.com/sunmeat/830e8cb963ff95c76498c67e3714b300

// https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code

#define WIN32_LEAN_AND_MEAN // для прискорення процесу збірки: https://stackoverflow.com/questions/11040133/what-does-defining-win32-lean-and-mean-exclude-exactly

#include <iostream>
#include <ws2tcpip.h> // тип WSADATA, функції WSAStartup, WSACleanup та багато чого іншого
using namespace std;

// реалізація бібліотеки Winsock 2 DLL
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015" // порт — це логічна конструкція, яка ідентифікує конкретний процес або тип мережевої служби - https://en.wikipedia.org/wiki/Port_(computer_networking)

#define PAUSE 1000

int main()
{
    setlocale(0, "");
    system("title SERVER SIDE");
    cout << "процес сервера запущено!\n";
    Sleep(PAUSE);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // ініціалізація Winsock
    WSADATA wsaData; // Структура WSADATA містить інформацію про реалізацію Windows Sockets: https://docs.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-wsadata
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Функція WSAStartup ініціює використання бібліотеки Winsock DLL процесом: https://firststeps.ru/mfc/net/socket/r.php?2
    if (iResult != 0) {
        cout << "WSAStartup не вдалося з помилкою: " << iResult << "\n";
        cout << "пiдключення Winsock.dll пройшло з помилкою!\n";
        return 1;
    }
    else {
        cout << "пiдключення Winsock.dll пройшло успiшно!\n";
        Sleep(PAUSE);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct addrinfo hints {}; // структура addrinfo використовується функцією getaddrinfo для зберігання інформації про адресу хоста: https://docs.microsoft.com/en-us/windows/win32/api/ws2def/ns-ws2def-addrinfoa
    hints.ai_family = AF_INET; // адреса сімейства протоколу Інтернету версії 4 (IPv4)
    hints.ai_socktype = SOCK_STREAM; // забезпечує послідовні, надійні, двосторонні, засновані на з'єднанні потоки байтів з механізмом передачі даних
    hints.ai_protocol = IPPROTO_TCP; // протокол передачі управління (TCP). Це можливе значення, коли член ai_family є AF_INET або AF_INET6, а член ai_socktype — SOCK_STREAM
    hints.ai_flags = AI_PASSIVE; // адреса сокета буде використовуватися в виклику функції "bind"

    // визначення адреси та порту сервера
    struct addrinfo* result = NULL;
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo не вдалося з помилкою: " << iResult << "\n";
        cout << "отримання адреси та порту сервера пройшло з помилкою!\n";
        WSACleanup(); // функція WSACleanup завершує використання бібліотеки Winsock 2 DLL (Ws2_32.dll): https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsacleanup
        return 2;
    }
    else {
        cout << "отримання адреси та порту сервера пройшло успiшно!\n";
        Sleep(PAUSE);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // створення сокету для підключення до сервера
    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == -1) {
        cout << "socket не вдалося створити з помилкою: " << WSAGetLastError() << "\n";
        cout << "створення сокета пройшло з помилкою!\n";
        freeaddrinfo(result);
        WSACleanup();

        return 3;
    }
    else {
        cout << "створення сокета на серверi пройшло успiшно!\n";
        Sleep(PAUSE);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // налаштування сокета для прослуховування
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen); // Функція bind асоціює локальну адресу з сокетом: https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-bind
    if (iResult == -1) {
        cout << "bind не вдалося з помилкою: " << WSAGetLastError() << "\n";
        cout << "вбудова сокета по IP-адресi пройшла з помилкою!\n";
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 4;
    }
    else {
        cout << "впровадження сокету по IP-адресi пройшло успiшно!\n";
        Sleep(PAUSE);
    }

    freeaddrinfo(result);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    iResult = listen(ListenSocket, SOMAXCONN); // функція listen ставить сокет у стан, коли він чекає вхідне підключення: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-listen
    if (iResult == -1) {
        cout << "listen не вдалося з помилкою: " << WSAGetLastError() << "\n";
        cout << "прослуховування iнформації вiд клiєнта не почалося. щось пiшло не так!\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 5;
    }
    else {
        cout << "починається прослуховування iнформацiї вiд клiєнта. будь ласка, запустiть клiєнтську програму! (client.exe)\n";
        // тут можна було б запустити якийсь прелоадер в окремому потоці
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // прийом сокета клієнта
    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL); // Функція accept дозволяє прийняти вхідне підключення на сокет: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-accept
    if (ClientSocket == INVALID_SOCKET) {
        cout << "accept не вдалося з помилкою: " << WSAGetLastError() << "\n";
        cout << "пiдключення з клiєнтською програмою не встановлено! сумно, сумно аж за край ((\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 6;
    }
    else {
        cout << "пiдключення з клiєнтською програмою встановлено успiшно!\n";
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // більше не потрібно зберігати сокет сервера
    closesocket(ListenSocket);

    // отримання даних до того моменту, поки з'єднання не буде завершено
    do {

        char message[DEFAULT_BUFLEN] = { 0 };

        iResult = recv(ClientSocket, message, DEFAULT_BUFLEN, 0); // функція recv використовується для читання вхідних даних: https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-recv
        message[iResult] = '\0';

        if (iResult > 0) {
            cout << "процес клiєнта надiслав повiдомлення: " << message << "\n";
            Sleep(PAUSE);
            cout << "байтiв отримано: " << iResult << "\n";
            Sleep(PAUSE);

            //////////////////////////////////////////////////////////////////////////////////////////////////////////

            // відправка відповіді назад відправнику
            const char* answer = "and hello from server!";
            cout << "процес сервера вiдправляє вiдповiдь: " << answer << "\n";

            int iSendResult = send(ClientSocket, answer, strlen(answer), 0); // функція send відправляє дані на підключений сокет: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-send

            Sleep(PAUSE);

            //////////////////////////////////////////////////////////////////////////////////////////////////////////

            if (iSendResult == SOCKET_ERROR) {
                cout << "send не вдалося з помилкою: " << WSAGetLastError() << "\n";
                cout << "упс, вiдправка (send) вiдповiдного повiдомлення не вiдбулася ((\n";
                closesocket(ClientSocket);
                WSACleanup();
                return 7;
            }
            else {
                cout << "байтiв вiдправлено: " << iSendResult << "\n";
                Sleep(PAUSE);
            }
        }
        else if (iResult == 0) {
            cout << "з'єднання закривається...\n";
            Sleep(PAUSE);
        }
        else {
            cout << "recv не вдалося з помилкою: " << WSAGetLastError() << "\n";
            cout << "упс, отримання (recv) вiдповiдного повiдомлення не вiдбулося ((\n";
            closesocket(ClientSocket);
            WSACleanup();
            return 8;
        }

    } while (iResult > 0);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // закриття з'єднання, оскільки роботу завершено
    iResult = shutdown(ClientSocket, SD_SEND); // функція shutdown вимикає надсилання або отримання даних на сокеті: https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-shutdown
    if (iResult == SOCKET_ERROR) {
        cout << "shutdown не вдалося з помилкою: " << WSAGetLastError() << "\n";
        cout << "упс, розрив з'єднання (shutdown) видав помилку ((\n";
        closesocket(ClientSocket);
        WSACleanup();
        return 9;
    }
    else {
        cout << "процес сервера припиняє свою роботу! до нових запускiв! :)\n";
    }

    // вивільнення памяті
    closesocket(ClientSocket);
    WSACleanup();
}
