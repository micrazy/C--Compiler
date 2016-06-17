.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
jr $ra

write:
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
move $v0, $0
jr $ra

main  :
subu $sp, $sp, 4
sw $ra, 0($sp)
subu $sp, $sp, 4
sw $fp, 0($sp)
addi $fp, $sp, 8
subu $sp, $sp, 1024


li $t0 , 0
subu $v1 ,$fp , 12
sw $t0, 0($v1)

li $t0 , 1
subu $v1 ,$fp , 16
sw $t0, 0($v1)

li $t0 , 0
subu $v1 ,$fp , 20
sw $t0, 0($v1)

addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t0 , $v0
subu $v1 ,$fp , 24
sw $t0, 0($v1)

label1  :


lw $t0, -20($fp)
lw $t1, -24($fp)
blt $t0 , $t1 , label2  
subu $v1 ,$fp , 20
sw $t0, 0($v1)
subu $v1 ,$fp , 24
sw $t1, 0($v1)

j label3  

label2  :


lw $t1, -12($fp)
lw $t2, -16($fp)
add $t0 , $t1 , $t2
subu $v1 ,$fp , 12
sw $t1, 0($v1)
subu $v1 ,$fp , 16
sw $t2, 0($v1)
subu $v1 ,$fp , 28
sw $t0, 0($v1)

lw $t0, -16($fp)
move $a0 , $t0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4

subu $v1 ,$fp , 16
sw $t0, 0($v1)

lw $t1, -16($fp)
move $t0 , $t1
subu $v1 ,$fp , 12
sw $t0, 0($v1)
subu $v1 ,$fp , 16
sw $t1, 0($v1)

lw $t1, -28($fp)
move $t0 , $t1
subu $v1 ,$fp , 16
sw $t0, 0($v1)
subu $v1 ,$fp , 28
sw $t1, 0($v1)

lw $t1, -20($fp)
addi $t0 , $t1 , 1
subu $v1 ,$fp , 20
sw $t0, 0($v1)

j label1  

label3  :


subu $sp, $fp, 8
lw $fp, 0($sp)
addi $sp, $sp, 4
lw $ra, 0($sp)
addi $sp, $sp, 4
li $t0 0
move $v0, $t0
jr $ra

