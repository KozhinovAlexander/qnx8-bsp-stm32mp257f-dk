Example:

MU library is intended to be used by other drivers but it can be also used in application, for example to communicate with Cortex-M core user firmware.

The library can be linked statically (BSP_root/src/lib/mu/mx8/aarch64/a-le/libmu-mx8.a)
or dynamically (BSP_root/src/lib/mu/mx8/aarch64/so-le/libmu-mx8.so).
For more information, refer to the "Using libaries" chapter in the "QNX Neutrino RTOS Programmer's Guide".

An example of a use case in pseudo code is provided below. In this example, the interrupts for the first three receive registers are disabled,
whereas the interrupt for the fourth and last receive register is enabled. The registers are then written in sequential order (n = 0, 1, 2, 3),
the write of the first three registers (n = 0, 1, 2) will not generate an interrupt to the receiving core, it is until the fourth and last receive
register (n = 3) is written that the receiving core will be notified by an interrupt. The receiving core can now service the interrupt and read back
the data transmitted on the four registers.:
@code

int main (void)
{
    /* Set thread privileges */
    if (-1 == ThreadCtl(_NTO_TCTL_IO_PRIV, 0)) {
        return -1;
    }
    /* Uses MU10A, other side (like Cortex-M) uses MU10B from LSIO subsystem */
    mu = imx_mu_init(IMX_MU10A_BASE, IMX_MU10A_IRQ);

    /* Dummy read with interrupt disabled - IMX_MU_FLAG_BLOCK */
    imx_mu_read(mu, 0, &value, 100, IMX_MU_FLAG_BLOCK);
    imx_mu_read(mu, 1, &value, 100, IMX_MU_FLAG_BLOCK);
    imx_mu_read(mu, 2, &value, 100, IMX_MU_FLAG_BLOCK);
    imx_mu_read(mu, 3, &value, 100, IMX_MU_FLAG_BLOCK);

    while (1) {
        /* Read Sequence :
            Processor receives the Receive 3 interrupt and starts reading the
            message transferred from the receive registers.
            After Receive Register 3 is read, the interrupt bit is cleared.
         */
        /* Continuously receive data in interrupt mode.
         Will sleep in InterruptWait() with timeout disabled. */
        if (imx_mu_read(mu, 3, &value3, 0, 0) ==  0) {
            /* Read Reg0 with interrupt disabled, timeout 100 us. */
            status = imx_mu_read(mu, 0, &value0, 100, IMX_MU_FLAG_BLOCK)
            if (status != 0) {
                /* Timeout. No data in Reg0 */
            }
            /* Read Reg1 with interrupt disabled */
            status = imx_mu_read(mu, 1, &value1, 100, IMX_MU_FLAG_BLOCK);
            if (status != 0) {
                /* Timeout. No data in Reg1 */
            }
            /* Read Reg2 with interrupt disabled */
            status = imx_mu_read(mu, 2, &value2, 100, IMX_MU_FLAG_BLOCK);
            if (status != 0) {
                /* Timeout. No data in Reg2 */
            }
            .
            .
            /* Do something with values or shared buffers
            if used for data transfers */
            .
            .
            /* The Processor writes the message information sequentially to its Transmit
               Registers 0, 1, 2.
             */
            status = imx_mu_send(mu, 0, value0, 100, IMX_MU_FLAG_BLOCK);
            if (status != 0) {
                /* Timeout. TX register is full. Other side probably did not
                 read previous data from TX register */
            }
            status = imx_mu_send(mu, 1, value1, 100, IMX_MU_FLAG_BLOCK);
            if (status != 0) {
                /* Timeout. TX register is full. Other side probably did not
                 read previous data from TX register */
            }
            status = imx_mu_send(mu, 2, value2, 100, IMX_MU_FLAG_BLOCK);
            if (status != 0) {
                /* Timeout. TX register is full. Other side probably did not
                 read previous data from TX register */
            }
            /* When the write to the Transmit Register 3 occurs, the RF3 bit of the xSR is set
               after synchronization, and it immediately trigger the Receive 3 interrupt to the
               other Processor. */
            status = imx_mu_send(mu, 3, value3, 100, IMX_MU_FLAG_BLOCK);
            if (status != 0) {
                /* Timeout. TX register is full. Other side probably did not
                read previous data from TX register */
            }
        }
    }
}
