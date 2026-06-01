void _start()
{
    __asm__("mov $60, %eax\n"
            "xor %edi, %edi \n"
            "syscall");
}
