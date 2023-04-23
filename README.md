# ИДЗ №2
## Амирханов Никита БПИ219
## Вариант 1
Задача о парикмахере. В тихом городке есть парикмахерская. Салон парикмахерской мал, работать в нем нем может только один парикмахер, обслуживающий одного посетителя. Есть несколько кресел для ожидания в очереди. Парикмахер всю жизнь обслуживает посетителей. Когда в салоне никого нет, он спит в кресле. Когда посетитель приходит и видит спящего парикмахера, он будет его, садится в кресло, «засыпая» на тот момент, пока парикмахер обслуживает его. Если посетитель приходит, а парикмахер занят, то он встает в очередь и «засыпает». После стрижки парикмахер сам провожает посетителя. Если есть ожидающие посетители, то парикмахер будит одного из них и ждет пока тот сядет в кресло парикмахера и начинает стрижку. Если никого нет, он снова садится в свое кресло и засыпает до прихода посетителя. Создать приложение, моделирующее рабочий день парикмахерской.

### Запуск кода
В задании на оценку 4:
```sh
gcc main.c -o main
./main <number of clients>
```
В заданиях 5-6:
```sh
gcc main.c -o main -lpthread -lrt
./main <number of clients>
```
В заданиях 7 и 9:
```sh
gcc cutter.c -o cutter
gcc client.c -o client
```
Затем в разных терминалах сначала запутсить `./cutter` затем сколько необходимо экземляров `./client`
В заданиях 8 и 10:
```sh
gcc cutter.c -o cutter -lpthread -lrt
gcc client.c -o client -lpthread -lrt
```
Затем в разных терминалах сначала запутсить `./cutter` затем сколько необходимо экземляров `./client`

### 4
Имеется приложение, которое с помощию функции `fork()` создает дочерние процессы. Каждый клиент идет к парикмахеру и закрыввает за собой семафор. Когда парикмахер  закончит с ним, он онкрывает семафор для следующих клиентов. В разделяемой памяти хранится `id` клиента - `getpid()`. При появлении сигнала `SIGINT` приложение завершает работу и убивает всех детей. Работает на семафорах и разделяемой памяти SYSTEM V
Пример работы:
```sh
./main 4
I am a Cutter
I am a client #2418!
[Cutter] No new clients
I am a client #2419!
I am a client #2420!
I am a client #2421!
[Cutter] No new clients
[2421] In queue for cutter
[2421] Going to cutter
[Cutter] new client #2421
[2419] In queue for cutter
[2418] In queue for cutter
[2420] In queue for cutter
[Cutter] finished client #2421
[Cutter] No new clients
[2419] Going to cutter
[Cutter] new client #2419
[Cutter] finished client #2419
[Cutter] No new clients
[2418] Going to cutter
[Cutter] new client #2418
[Cutter] finished client #2418
[Cutter] No new clients
[2420] Going to cutter
[Cutter] new client #2420
[Cutter] finished client #2420
[Cutter] No new clients
[Cutter] No new clients
^C[Cutter] SIGINT Detected!
[Cutter] Killing child #2418
[Cutter] Killing child #2419
[Cutter] Killing child #2420
[Cutter] Killing child #2421
```
### 5
Имеется приложение, которое с помощию функции `fork()` создает дочерние процессы. Каждый клиент идет к парикмахеру и закрыввает за собой семафор. Когда парикмахер  закончит с ним, он онкрывает семафор для следующих клиентов. В разделяемой памяти хранится `id` клиента - `getpid()`. При появлении сигнала `SIGINT` приложение завершает работу и убивает всех детей. По работе схожа с пердыдущей задачей, но использует неименованные семафоры и разделяемую память POSIX
Пример работы:
```sh
./main 3
Object is open: name = shared-memory, id = 0x3
Memory size set and = 40
I am a client #2486!
I am a client #2487!
I am a Cutter
[Cutter] No new clients
I am a client #2488!
[2488] In queue for cutter
[2488] Going to cutter
[2487] In queue for cutter
[Cutter] new client #2488
[2486] In queue for cutter
[Cutter] finished client #2488
[Cutter] No new clients
[2487] Going to cutter
[Cutter] new client #2487
[Cutter] finished client #2487
[Cutter] No new clients
[2486] Going to cutter
[Cutter] new client #2486
[Cutter] finished client #2486
[Cutter] No new clients
[Cutter] No new clients
^C[Cutter] SIGINT Detected!
[Cutter] Killing child #2486
[Cutter] Killing child #2487
[Cutter] Killing child #2488
```
### 6
Имеется приложение, которое с помощию функции `fork()` создает дочерние процессы. Каждый клиент идет к парикмахеру и закрыввает за собой семафор. Когда парикмахер  закончит с ним, он онкрывает семафор для следующих клиентов. В разделяемой памяти хранится `id` клиента - `getpid()`. При появлении сигнала `SIGINT` приложение завершает работу и убивает всех детей. По работе схожа с пердыдущей задачей, но использует именованные семафоры и разделяемую память POSIX
```sh
./main 2
Object is open: name = shared-memory, id = 0x3
Memory size set and = 16
I am a client #2500!
I am a Cutter
[Cutter] No new clients
I am a client #2501!
[2500] In queue for cutter
[2500] Going to cutter
[2501] In queue for cutter
[Cutter] new client #2500
[Cutter] finished client #2500
[Cutter] No new clients
[2501] Going to cutter
[Cutter] new client #2501
[Cutter] finished client #2501
[Cutter] No new clients
[Cutter] No new clients
^C[Cutter] SIGINT Detected!
[Cutter] Killing child #2500
[Cutter] Killing child #2501
```
### 7
Есть две программы. `cutter` и `client`. Сначала необходимо запустить приложение парикмахера, затем сколько угодно клиентов. Каждый клиент идет к парикмахеру и закрыввает за собой семафор. Когда парикмахер  закончит с ним, он онкрывает семафор для следующих клиентов, после чего умирает. В разделяемой памяти хранится `id` клиента - `getpid()`. Работают на SYSTEM V.
```sh
./cutter | ./client | ./client
I am a Cutter
[Cutter] No new clients
[Cutter] new client #2524
[Cutter] finished client #2524
[Cutter] No new clients
[Cutter] new client #2525
[Cutter] finished client #2525
[Cutter] No new clients
^C[Cutter] SIGINT Detected!

I am a client #2524!
[2524] Going to cutter

I am a client #2525!
[2525] Going to cutter
```
### 8
Есть две программы. `cutter` и `client`. Сначала необходимо запустить приложение парикмахера, затем сколько угодно клиентов. Каждый клиент идет к парикмахеру и закрыввает за собой семафор. Когда парикмахер  закончит с ним, он онкрывает семафор для следующих клиентов, после чего умирает. В разделяемой памяти хранится `id` клиента - `getpid()` и объект семафора. Работают на POSIX.
```sh
./cutter | ./client
Object is open: name = shared-memory, id = 0x3
Memory size set and = 40
I am a Cutter
[Cutter] No new clients
[Cutter] No new clients
[Cutter] new client #2596
[Cutter] finished client #2596
[Cutter] No new clients
[Cutter] No new clients
[Cutter] SIGINT Detected!

Object is open: name = shared-memory, id = 0x3
Memory size set and = 40
I am a client #2596!
[2596] Going to cutter
```
### 9
Есть две программы. `cutter` и `client`. Сначала необходимо запустить приложение парикмахера, затем сколько угодно клиентов. При запуске парикмахера он начинает слушать входящие сообщения через систему сообщений SYSTEM V. Прри запуске клиента, он идет к парикмахеру и закрыввает за собой семафор, и отправляет сообщение парикмахеру со своим `pid`. Когда парикмахер заканчивает с клиентом, он передает ему сообщение назад, и клиент уходит, открывает семафор. Сделано на очереди сообщений и семафорах SYSTEM V.
```sh
./cutter ./client
Created queue with id: 0
Created queue with id: 32769
[Cutter] New client with id: 2619
[Cutter] Finished client with id: 2619
^C[Cutter] SIGINT Detected!

Created queue with id: 32769
Created queue with id: 0
[2619] I am new client, waiting in queue
[2619] Got to cutter
[2619] Leaving cutter
```
### 10
Есть две программы. `cutter` и `client`. Сначала необходимо запустить приложение парикмахера, затем сколько угодно клиентов. При запуске парикмахера он начинает слушать входящие сообщения через каналы POSIX. Прри запуске клиента, он идет к парикмахеру и закрыввает за собой семафор, и отправляет сообщение парикмахеру со своим `pid`. Когда парикмахер заканчивает с клиентом, он передает ему сообщение назад, и клиент уходит, открывает семафор. Была добавлена разделяемая паять для получения доступа к общему семафору. Все на POSIX.
```sh
./cutter | ./client
Object is open: name = shared-memory-10, id = 0x4
Memory size set and = 40
[Cutter] New client with id: 2627
[Cutter] Finished client with id: 2627
^C[Cutter] SIGINT Detected!

Object is open: name = shared-memory-10, id = 0x3
Memory size set and = 40
[2627] I am new client, waiting in queue
[2627] Got to cutter
[2627] Leaving cutter
```

