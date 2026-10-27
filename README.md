# Smart-Blood-Bank-Management-System..

cp kernel.elf iso/boot/kernel.elf
grub-mkrescue -o raktsetu-kernel.iso iso
qemu-system-i386 -cdrom raktsetu-kernel.iso

this is for the running of the kernel.