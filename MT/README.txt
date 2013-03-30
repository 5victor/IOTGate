1.RTS/CTS流控制关系，需要关MT的流控。

Components\mt\MT_Uart.c

void MT_UartInit ()
{
  halUARTCfg_t uartConfig;

  /* Initialize APP ID */
  App_TaskID = 0;

  /* UART Configuration */
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = MT_UART_DEFAULT_BAUDRATE;
  uartConfig.flowControl          = 0;//MT_UART_DEFAULT_OVERFLOW;
  uartConfig.flowControlThreshold = 0;//MT_UART_DEFAULT_THRESHOLD;

2.这次提交的能正常与cc2530的串口通信了。如果收不到数据，把两个设备都重启看看。

