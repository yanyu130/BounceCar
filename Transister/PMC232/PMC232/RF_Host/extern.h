
System_Clock	EQU	8000000	//	Used at UART, PS2, ...
Program_Mode	EQU	0			//

#if	defined(ADCC)
	#if defined(ADCR)
		ADCRH	EQU		ADCR
		ADCRL	EQU		0
	#else
		ADCR	EQU		ADCRH
	#endif
#endif



