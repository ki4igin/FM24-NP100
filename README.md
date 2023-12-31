# Радар 24 ГГц на базе платы FM24-MP100
На плате расположена антенная система, приемно-передающий модуль (ППМ) на 24 ГГц с генератором управляемым напряжением (ГУН), каскад операционных усилителей для промежуточной частоты, микроконтроллер STM32F303, импульсный преобразователь.

## Принцип работы
МК с помощью встроенного ЦАП формирует модуляционное напряжения, которое поступает на вход ГУН ППМ. ППМ посредством передающей антенной системы излучает ЧМ сигнал. Он отражается от объекта, поступает на приемную антенную систему, далее на вход приемника ППМ. Отраженный сигнал поступает на вход смесителя ППМ, на опорный вход которого поступает модуляционное напряжение. На выходе смесителя формируется две квадратуры сигнала промежуточной частоты (сигнала разностной частоты). Два сигнала усиливаются на соответствующих каскадах ОУ и поступают на входы встроенных в МК АЦП1 и АЦП2. Таким образом, на выходе АЦП1 формируется действительная часть аналитического сигнала, а на выходе АЦП2 - мнимая.

## Работа с модулем
Питание модуля - постоянное напряжение 4-8 В, ток потребление более 100 мА. Для обмена информацией с МК используется интерфейс UART.

Параметры UART:
- Скорость: 115200 бод
- Количество стоп-бит: 1
- Бит паритета: нет

Исходные настройки
- Крутизна ГУН - 760 МГц
- Форма модуляционного напряжения - несимметричная пила
- Амплитуда модуляционного напряжения - 200 попугаев
- Частота модуляционного напряжения - 30.517578125 Гц
- Частота дискретизации АЦП - 500 кГц

### Подключение
Для подключения модуля используется разъем с цветными проводами:
1. VDD - вход питания (4-8 В)
2. GND - общий
3. 3V3_OUT - выход внутреннего импульсного преобразователя напряжения (это ВЫХОД)
4. UART_TX - выход передатчика UART
5. UART_RX - вход приемника UART
6. NC - пока не используется

### Команды управления
Для управления модулем используются команды. Каждая команда состоит из 4-х последовательных байт. Если временной интервал между байтами команды больше 200 битовых импульсов, то байты не обрабатываются. Формат команд следующий:
- 1-й байт.: идентификатор команды
- 2-4 байты: аргумент команды (3 байтное число, первый байт младший)

| Наименование | Id (1&nbsp;байт) | Аргумент (3 байта, первый байт младший) |
| --- | --- | --- |
| Собрать данные | 0x01 | Количество отчетов, от 1 до 8192 |
| Остановить сбор данных | 0x02 | - |
| Сброс МК | 0x03 | - |
| Запрос тестового ответа МК | 0x04 | - |
| Задать форму модуляционного напряжения | 0x05 | Код формы сигнала: <br> 1 - симметричная пила <br> 2 - несимметричная пила <br> 3 - синус <br> 4 - постоянный сигнал |
| Задать амплитуду модуляционного напряжения | 0x06 | Значение амплитуды в мВ (для постоянного сигнала задается его значение) |
| Задать девиацию частоты | 0x07 | Значение девиации частоты в кГц, от 0 до 200_000 |
| Задать крутизну ГУН | 0x08 | Значение крутизны ГУН в кГц/В, до 2000_000 |
| Задать частоту дискретизации АЦП | 0x09 | Код частоты: <br> 0 - 2 МГц <br> 1 - 1 МГц <br> 2 - 500 кГц <br> 3 - 250 кГц <br> 4 - 125 кГц |
| Задать частоту модуляционного напряжения | 0x0A | Код частоты: <br> 09 - 3.98625 кГц <br> 10 - 1.953125 кГц <br> 11 - 976.5625 Гц <br> 12 - 488.28125 Гц <br> 13 - 244.140625 Гц <br> 14 - 122.0703125 Гц <br> 15 - 65.03515625 Гц <br> 16 - 30.517578125 Гц <br> 17 - 15.2587890625 Гц |

### Примеры команд
- Собрать 640 отчетов:
`0x01 0x80 0x02 0x00`

- Задать частоту дискретизации АЦП 500 кГц:
`0x09 0x02 0x00 0x00`

- Задать частоту модуляционного напряжения 15.2587890625 Гц:
`0x0A 0x11 0x00 0x00`

### Примечания к командам
- Если аргумент команды выходит за ограничения, то команда игнорируется
- Если команда сбора данных приходит во время сбора данных, то она игнорируется
- Тестовый ответ содержит следующие 4 байта: `0x04 0x11 0x22 0x33`, МК его отсылает при включении и в ответ на соответствующую команду
- Частота дискретизации АЦП и частота модуляционного напряжения подобраны так чтобы их отношение составляло $2^n$ 

### Формат пакета данных
После отправки команды "Собрать данные" МК отсылает собранные данные в следующем формате

Размер | Наименование |
--- | --- |
u8 | идентификатор (равен 0x01) |
u24 | размер массива слов данных в байтах (1&#x2011;й байт младший) |
u32[] | массив слов данных |

Каждое 32-битное слово данных в массиве содержит два 16-битных отчета:
- старшие 16 бит от АЦП1, действительная часть аналитического сигнала (1&#x2011;й байт младший)
- младшие 16 бит от АЦП2, мнимая часть аналитического сигнала (1&#x2011;й байт младший)

## Компиляция и прошивка МК
Для программирования МК интерфейс SWD

### Вариант 1 - Использование GCC и STM32 ST-LINK Utility
Требования к ПО:
- make
- arm gnu toolchain
- ST-LINK Utility

Команда для компиляции проекта, если папка компилятора arm-none-eabi-gcc содержится в переменной PATH:

```sh
make -C GCC
```

Команда для компиляции проекта, с указанием папки компилятора arm-none-eabi-gcc:

```sh
make -C GCC CC_PATH="путь до папки компилятора"
```

Скрипты для прошивки и стирания МК через STM32 ST-LINK Utility: 
```sh
./GCC/mcu_flash.sh
./GCC/mcu_erase.sh
```

### Вариант 2 - Использование MDK-ARM
Требования:
- MDK-ARM (Keil uVision5) с компилятором 6-й версии (компиляция с 5-й версией не проверялась)

Открыть проект `./MDR-ARM/FM24-NP100.uvprojx`, скомпилировать, прошить
