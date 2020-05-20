//rozdzielczosc wyswietlacza LCD (wiersze/kolumny)
#define LCD_Y 2 //liczba wierszy wyświetlacza LCD
#define LCD_X 16 //liczba kolumn wyświetlacza LCD

//tu ustalamy za pomocą zera lub jedynki czy sterujemy pinem Rw (zapis czy odczyt)
#define USE_RW 1 //0 - pin zwarty do GND, 1- pin do uC

//tu konfigurujemy port i piny, do których podłączamy linie D7-D4 LCD
#define LCD_D7PORT C
#define LCD_D7 5
#define LCD_D6PORT C
#define LCD_D6 4
#define LCD_D5PORT C
#define LCD_D5 3
#define LCD_D4PORT C

//definiowanie pinów procesora do któych podłączone są sygnały RS, RW, E
#define LCD_RSPORT B
#define LCD_RS 2 
#define LCD_RWPORT B
#define LCD_RW 1
#define LCD_EPORT B
#define LCD_E 0

//definiowanie adresów w DDRAM dla różnych wyświetlaczy
//inne w wyswietlaczach 2 wierszowych, inne w 4 wierszowych
#if ( (LCD_Y == 4 ) && (LCD_X == 20) )
#define LCD_LINE1 0x00 //adres 1 znaku 1 wiersza
#define LCD_LINE2 0x28 //adres 1 znaku 2 wiersza
#define LCD_LINE3 0x14 //adres 1 znaku 3 wiersza
#define LCD_LINE4 0x54 //adres 1 znaku 4 wiersza
#else
#define LCD_LINE1 0x00 //adres 1 znaku 1 wiersza
#define LCD_LINE2 0x40 //adres 1 znaku 2 wiersza
#define LCD_LINE3 0x10 //adres 1 znaku 3 wiersza
#define LCD_LINE4 0x50 //adres 1 znaku 4 wiersza
#endif

//inicjalizacja pinów portów ustalonych do podłączenia z wyświetlaczem LCD
//ustawienie wszystkich jako wyjścia
DDR(LCD_D7PORT) |= (1<<LCD_D7);
DDR(LCD_D6PORT) |= (1<<LCD_D6);
DDR(LCD_D5PORT) |= (1<<LCD_D5);
DDR(LCD_D4PORT) |= (1<<LCD_D4);
DDR(LCD_RSPORT) |= (1<<LCD_RS);
DDR(LCD_EPORT)  |= (1<<LCD_E);
#if USE_RW == 1 //kompilacja warunkowa, gdy uzywamy RW
DDR (LCD_RWPORT) |= (1<<LCD_RW);
#endif

//wyzerowanie wszystkich linii sterujących
PORT(LCD_RSPORT) &= ~(1<<LCD_RS);
PORT(LCD_EPORT) &= ~(1<<LCD_E);
#if USE_RW == 1 //kompilacja warunkowa przy uzyciu RW
PORT(LCD_RWPORT) &= ~(1<<LCD_RW);
#endif

//makrodefinicje operacji na sygnałach sterujących RS,RW oraz E
//stan wysoki na linii RS
#define SET_RS PORT(LCD_RSPORT) |= (1<<LCD_RS)
//stan niski na linii RS
#define CLR_RS PORT(LCD_RSPORT) |= ~(1<<LCD_RS)
//stan wysoki na RW - odczyt
#define SET_RW PORT(LCD_RWPORT) |= (1<<LCD_RW)
//stan niski na RW - zapis
#define CLR_RW PORT(LCD_RWPORT) &= (1<<LCD_RW)

//stan wysoki na linii E
#define SET_E PORT(LCD_EPORT) |= (1<<LCD_E)
//stan niski na linii E
#define CLR_E PORT(LCD_EPORT) |= ~(1<<LCD_E)


//funkcja przesyłająca połówkę bajtu do LCD, aby trafily odpowiednio na D7-D4
//static oznacza ze nie bedzie ona udostepniona do innych modułów
//inline oznacza aby kompilator przerobił tę funkcję na makro
//argumentem funkcji jest jeden bit o nazwie data
static inline void lcd_sendHalf(uint8_t data)
{
	if (data&(1<<0)) PORT(LCD_D4PORT) |= (1<<LCD_D4);
	else PORT(LCD_D4PORT) &= ~(1<<LCD_D4);
	
	if (data&(1<<1)) PORT(LCD_D5PORT) |= (1<<LCD_D5);
	else PORT(LCD_D5PORT) &= ~(1<<LCD_D5);
	
	if (data&(1<<2)) PORT(LCD_D6PORT) |= (1<<LCD_D6);
	else PORT(LCD_D6PORT) &= ~(1<<LCD_D6);
	
	if (data&(1<<3)) PORT(LCD_D7PORT) |= (1<<LCD_D7);
	else PORT(LCD_D7PORT) &= ~(1<<LCD_D7);
}

//funkcja do przesyłania całego bajtu
void _lcd_write_byte(unsigned char_data)
{
	//ustawienie pinów portu LCD D4-D7 jako wyjścia
	data_dir_out();
	
	#if USE_RW == 1
	CLR_RW;
	#endif
	SET_E;
	lcd_sendHalf(_data>>4); //wysłanie starszej części bajtu danych D7-D4
	CLR_E;
	
	SET_E;
	lcd_sendHalf(_data); //wysłanie młodszej częsci bajtu danych D3-D0
	CLE_E;
	
	#if USE_RW == 1
	while (check_BF() & (1<<7) );
	#else 
	_delay_us(120); //opoznineie aby sterownik zdazyl wykonac wewnetrzne operacje po odebraniu bajtu
	#endif 
}

                
