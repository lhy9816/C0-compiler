.data
	$String0:.asciiz "BAD RESULT IN fib\n"
	$String1:.asciiz "FUNC WITH PARA BUT NO RETURN\n"
	$String2:.asciiz "\n"
	$String3:.asciiz " !#$%&'()*+,-./0123456789:;<=>?@ ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n"
	$String4:.asciiz "none1 ok\n"
	$String5:.asciiz "none2 ok\n"
	$String6:.asciiz "WRONG INPUT IN CASE3\n"
	$String7:.asciiz "WRONG INPUT!\n"
	$String8:.asciiz "\n"
.text
	addi	$gp	,$gp	,0x10000
	addu	$fp	,$sp	,$zero
	j	main	
fib:
	addi	$sp	,$sp	,-92
	sw	$t0	,68($sp)
	sw	$ra	,4($sp)
	move	$t0	,$a0
	li	$t9	,1
	bne	$t0	,$t9	,LABEL1
	li	$v0	,1
	j	fib_end	
LABEL1:
	li	$t9	,2
	bne	$t0	,$t9	,LABEL3
	li	$v0	,2
	j	fib_end	
LABEL3:
	addiu	$s0	,$t0	,-1
	move	$a0	,$s0
	jal	fib	
	move	$s0	,$v0
	addiu	$s1	,$t0	,-2
	move	$a0	,$s1
	sw	$s0	,84($sp)
	jal	fib	
	move	$s0	,$v0
	lw	$s1	,84($sp)
	addu	$s2	,$s1	,$s0
	move	$v0	,$s2
	j	fib_end	
LABEL4:
	j	fib_end	
	la	$a0	,$String0
	li	$v0	,4
	syscall		
	li	$v0	,-1
fib_end:
	lw	$ra	,4($sp)
	lw	$t0	,68($sp)
	addi	$sp	,$sp	,92
	jr	$ra	
fun0:
	addi	$sp	,$sp	,-352
	sw	$ra	,4($sp)
	li	$t9	,344
	addu	$t9	,$sp	,$t9
	li	$t8	,43
	sw	$t8	,0($t9)
	li	$t9	,340
	addu	$t9	,$sp	,$t9
	li	$t8	,45
	sw	$t8	,0($t9)
	li	$t9	,336
	addu	$t9	,$sp	,$t9
	li	$t8	,42
	sw	$t8	,0($t9)
	li	$t9	,332
	addu	$t9	,$sp	,$t9
	li	$t8	,47
	sw	$t8	,0($t9)
	li	$t9	,328
	addu	$t9	,$sp	,$t9
	li	$t8	,95
	sw	$t8	,0($t9)
	li	$t9	,324
	addu	$t9	,$sp	,$t9
	li	$t8	,48
	sw	$t8	,0($t9)
	li	$t9	,320
	addu	$t9	,$sp	,$t9
	li	$t8	,49
	sw	$t8	,0($t9)
	li	$t9	,316
	addu	$t9	,$sp	,$t9
	li	$t8	,50
	sw	$t8	,0($t9)
	li	$t9	,312
	addu	$t9	,$sp	,$t9
	li	$t8	,51
	sw	$t8	,0($t9)
	li	$t9	,308
	addu	$t9	,$sp	,$t9
	li	$t8	,52
	sw	$t8	,0($t9)
	li	$t9	,304
	addu	$t9	,$sp	,$t9
	li	$t8	,53
	sw	$t8	,0($t9)
	li	$t9	,300
	addu	$t9	,$sp	,$t9
	li	$t8	,54
	sw	$t8	,0($t9)
	li	$t9	,296
	addu	$t9	,$sp	,$t9
	li	$t8	,55
	sw	$t8	,0($t9)
	li	$t9	,292
	addu	$t9	,$sp	,$t9
	li	$t8	,56
	sw	$t8	,0($t9)
	li	$t9	,288
	addu	$t9	,$sp	,$t9
	li	$t8	,57
	sw	$t8	,0($t9)
	li	$t9	,284
	addu	$t9	,$sp	,$t9
	li	$t8	,97
	sw	$t8	,0($t9)
	li	$t9	,280
	addu	$t9	,$sp	,$t9
	li	$t8	,98
	sw	$t8	,0($t9)
	li	$t9	,276
	addu	$t9	,$sp	,$t9
	li	$t8	,99
	sw	$t8	,0($t9)
	li	$t9	,272
	addu	$t9	,$sp	,$t9
	li	$t8	,100
	sw	$t8	,0($t9)
	li	$t9	,268
	addu	$t9	,$sp	,$t9
	li	$t8	,101
	sw	$t8	,0($t9)
	li	$t9	,264
	addu	$t9	,$sp	,$t9
	li	$t8	,102
	sw	$t8	,0($t9)
	li	$t9	,260
	addu	$t9	,$sp	,$t9
	li	$t8	,103
	sw	$t8	,0($t9)
	li	$t9	,256
	addu	$t9	,$sp	,$t9
	li	$t8	,104
	sw	$t8	,0($t9)
	li	$t9	,252
	addu	$t9	,$sp	,$t9
	li	$t8	,105
	sw	$t8	,0($t9)
	li	$t9	,248
	addu	$t9	,$sp	,$t9
	li	$t8	,106
	sw	$t8	,0($t9)
	li	$t9	,244
	addu	$t9	,$sp	,$t9
	li	$t8	,107
	sw	$t8	,0($t9)
	li	$t9	,240
	addu	$t9	,$sp	,$t9
	li	$t8	,108
	sw	$t8	,0($t9)
	li	$t9	,236
	addu	$t9	,$sp	,$t9
	li	$t8	,109
	sw	$t8	,0($t9)
	li	$t9	,232
	addu	$t9	,$sp	,$t9
	li	$t8	,110
	sw	$t8	,0($t9)
	li	$t9	,228
	addu	$t9	,$sp	,$t9
	li	$t8	,111
	sw	$t8	,0($t9)
	li	$t9	,224
	addu	$t9	,$sp	,$t9
	li	$t8	,112
	sw	$t8	,0($t9)
	li	$t9	,220
	addu	$t9	,$sp	,$t9
	li	$t8	,113
	sw	$t8	,0($t9)
	li	$t9	,216
	addu	$t9	,$sp	,$t9
	li	$t8	,114
	sw	$t8	,0($t9)
	li	$t9	,212
	addu	$t9	,$sp	,$t9
	li	$t8	,115
	sw	$t8	,0($t9)
	li	$t9	,208
	addu	$t9	,$sp	,$t9
	li	$t8	,116
	sw	$t8	,0($t9)
	li	$t9	,204
	addu	$t9	,$sp	,$t9
	li	$t8	,117
	sw	$t8	,0($t9)
	li	$t9	,200
	addu	$t9	,$sp	,$t9
	li	$t8	,118
	sw	$t8	,0($t9)
	li	$t9	,196
	addu	$t9	,$sp	,$t9
	li	$t8	,119
	sw	$t8	,0($t9)
	li	$t9	,192
	addu	$t9	,$sp	,$t9
	li	$t8	,120
	sw	$t8	,0($t9)
	li	$t9	,188
	addu	$t9	,$sp	,$t9
	li	$t8	,121
	sw	$t8	,0($t9)
	li	$t9	,184
	addu	$t9	,$sp	,$t9
	li	$t8	,122
	sw	$t8	,0($t9)
	li	$t9	,180
	addu	$t9	,$sp	,$t9
	li	$t8	,65
	sw	$t8	,0($t9)
	li	$t9	,176
	addu	$t9	,$sp	,$t9
	li	$t8	,66
	sw	$t8	,0($t9)
	li	$t9	,172
	addu	$t9	,$sp	,$t9
	li	$t8	,67
	sw	$t8	,0($t9)
	li	$t9	,168
	addu	$t9	,$sp	,$t9
	li	$t8	,68
	sw	$t8	,0($t9)
	li	$t9	,164
	addu	$t9	,$sp	,$t9
	li	$t8	,69
	sw	$t8	,0($t9)
	li	$t9	,160
	addu	$t9	,$sp	,$t9
	li	$t8	,70
	sw	$t8	,0($t9)
	li	$t9	,156
	addu	$t9	,$sp	,$t9
	li	$t8	,71
	sw	$t8	,0($t9)
	li	$t9	,152
	addu	$t9	,$sp	,$t9
	li	$t8	,72
	sw	$t8	,0($t9)
	li	$t9	,148
	addu	$t9	,$sp	,$t9
	li	$t8	,73
	sw	$t8	,0($t9)
	li	$t9	,144
	addu	$t9	,$sp	,$t9
	li	$t8	,74
	sw	$t8	,0($t9)
	li	$t9	,140
	addu	$t9	,$sp	,$t9
	li	$t8	,75
	sw	$t8	,0($t9)
	li	$t9	,136
	addu	$t9	,$sp	,$t9
	li	$t8	,76
	sw	$t8	,0($t9)
	li	$t9	,132
	addu	$t9	,$sp	,$t9
	li	$t8	,77
	sw	$t8	,0($t9)
	li	$t9	,128
	addu	$t9	,$sp	,$t9
	li	$t8	,78
	sw	$t8	,0($t9)
	li	$t9	,124
	addu	$t9	,$sp	,$t9
	li	$t8	,79
	sw	$t8	,0($t9)
	li	$t9	,120
	addu	$t9	,$sp	,$t9
	li	$t8	,80
	sw	$t8	,0($t9)
	li	$t9	,116
	addu	$t9	,$sp	,$t9
	li	$t8	,81
	sw	$t8	,0($t9)
	li	$t9	,112
	addu	$t9	,$sp	,$t9
	li	$t8	,82
	sw	$t8	,0($t9)
	li	$t9	,108
	addu	$t9	,$sp	,$t9
	li	$t8	,83
	sw	$t8	,0($t9)
	li	$t9	,104
	addu	$t9	,$sp	,$t9
	li	$t8	,84
	sw	$t8	,0($t9)
	li	$t9	,100
	addu	$t9	,$sp	,$t9
	li	$t8	,85
	sw	$t8	,0($t9)
	li	$t9	,96
	addu	$t9	,$sp	,$t9
	li	$t8	,86
	sw	$t8	,0($t9)
	li	$t9	,92
	addu	$t9	,$sp	,$t9
	li	$t8	,87
	sw	$t8	,0($t9)
	li	$t9	,88
	addu	$t9	,$sp	,$t9
	li	$t8	,88
	sw	$t8	,0($t9)
	li	$t9	,84
	addu	$t9	,$sp	,$t9
	li	$t8	,89
	sw	$t8	,0($t9)
	li	$t9	,80
	addu	$t9	,$sp	,$t9
	li	$t8	,90
	sw	$t8	,0($t9)
	li	$a0	,95
	li	$v0	,11
	syscall		
	li	$t9	,2
	move	$a0	,$t9
	jal	fib	
	move	$s0	,$v0
	li	$t8	,86
	sub	$t9	,$t8	,$s0
	sll	$t9	,$t9	,2
	addu	$t9	,$sp	,$t9
	lw	$s1	,0($t9)
	move	$v0	,$s1
fun0_end:
	lw	$ra	,4($sp)
	addi	$sp	,$sp	,352
	jr	$ra	
fun1:
	addi	$sp	,$sp	,-164
	sw	$t0	,68($sp)
	sw	$t1	,64($sp)
	sw	$ra	,4($sp)
	move	$t1	,$a0
	li	$t9	,4
	addu	$t9	,$gp	,$t9
	li	$t8	,1
	sw	$t8	,0($t9)
	li	$t9	,8
	addu	$t9	,$gp	,$t9
	li	$t8	,-1
	sw	$t8	,0($t9)
	j	LABEL10	
	li	$t0	,1
	j	LABEL17	
LABEL6:
	j	LABEL10	
	li	$t0	,2
	j	LABEL17	
LABEL8:
	j	LABEL10	
	li	$t0	,3
	j	LABEL17	
LABEL10:
	li	$t9	,4
	addu	$t9	,$t9	,$gp
	lw	$s0	,0($t9)
	li	$t9	,8
	addu	$t9	,$t9	,$gp
	lw	$s1	,0($t9)
	addu	$s2	,$s0	,$s1
	li	$t9	,4
	ble	$s2	,$t9	,LABEL12
	li	$t0	,4
	j	LABEL17	
LABEL12:
	li	$s0	,0
	li	$t9	,4
	addu	$t9	,$t9	,$gp
	lw	$s1	,0($t9)
	mul	$s2	,$s1	,10
	addiu	$s3	,$s1	,4
	move	$a0	,$s3
	sw	$s0	,140($sp)
	sw	$s2	,132($sp)
	jal	fib	
	move	$s0	,$v0
	lw	$s1	,132($sp)
	addu	$s2	,$s1	,$s0
	lw	$s3	,140($sp)
	blt	$s3	,$s2	,LABEL14
	li	$t0	,5
	j	LABEL17	
LABEL14:
	li	$t9	,4
	addu	$t9	,$t9	,$gp
	lw	$s0	,0($t9)
	move	$s1	,$s0
	li	$s2	,13
	mul	$s3	,$s0	,10
	li	$t9	,2
	move	$a0	,$t9
	sw	$s1	,92($sp)
	sw	$s2	,104($sp)
	sw	$s3	,96($sp)
	jal	fib	
	move	$s0	,$v0
	lw	$s1	,92($sp)
	addu	$s2	,$s1	,$s0
	move	$a0	,$s2
	jal	fib	
	move	$s0	,$v0
	lw	$s1	,96($sp)
	addu	$s2	,$s1	,$s0
	lw	$s3	,104($sp)
	beq	$s3	,$s2	,LABEL16
	li	$t0	,7
	j	LABEL17	
LABEL16:
	li	$t0	,6
LABEL17:
	la	$a0	,$String1
	li	$v0	,4
	syscall		
	addu	$s0	,$t0	,$t1
	move	$a0	,$s0
	li	$v0	,1
	syscall		
fun1_end:
	lw	$ra	,4($sp)
	lw	$t1	,64($sp)
	lw	$t0	,68($sp)
	addi	$sp	,$sp	,164
	jr	$ra	
fun2:
	addi	$sp	,$sp	,-124
	sw	$t0	,68($sp)
	sw	$t1	,64($sp)
	sw	$ra	,4($sp)
	li	$t1	,2
	li	$t9	,112
	addu	$t9	,$sp	,$t9
	li	$t8	,2
	sw	$t8	,0($t9)
	li	$t9	,3
	move	$a0	,$t9
	jal	fib	
	move	$s0	,$v0
	li	$t9	,54
	sub	$s1	,$t9	,$s0
	move	$t9	,$t1
	li	$t8	,30
	sub	$t9	,$t8	,$t9
	sll	$t9	,$t9	,2
	addu	$t9	,$sp	,$t9
	lw	$s2	,0($t9)
	div		,$t1	,$s2
	mflo	$s3	
	addiu	$s4	,$s3	,1
	mul	$s5	,$s1	,$s4
	li	$t9	,0
	sub	$t0	,$t9	,$s5
	move	$a0	,$t0
	li	$v0	,1
	syscall		
	move	$a0	,$s2
	li	$v0	,1
	syscall		
	li	$a0	,966
	li	$v0	,1
	syscall		
	la	$a0	,$String2
	li	$v0	,4
	syscall		
	li	$a0	,2
	li	$v0	,1
	syscall		
	la	$a0	,$String3
	li	$v0	,4
	syscall		
fun2_end:
	lw	$ra	,4($sp)
	lw	$t1	,64($sp)
	lw	$t0	,68($sp)
	addi	$sp	,$sp	,124
	jr	$ra	
none1:
	addi	$sp	,$sp	,-68
	sw	$ra	,4($sp)
	la	$a0	,$String4
	li	$v0	,4
	syscall		
none1_end:
	lw	$ra	,4($sp)
	addi	$sp	,$sp	,68
	jr	$ra	
none2:
	addi	$sp	,$sp	,-68
	sw	$ra	,4($sp)
	la	$a0	,$String5
	li	$v0	,4
	syscall		
none2_end:
	lw	$ra	,4($sp)
	addi	$sp	,$sp	,68
	jr	$ra	
do_while_test:
	addi	$sp	,$sp	,-96
	sw	$t0	,68($sp)
	sw	$ra	,4($sp)
	move	$t0	,$a0
	li	$s0	,0
	sw	$s0	,0($gp)
LABEL18:
	lw	$t9	,0($gp)
	addiu	$t9	,$t9	,1
	sll	$t9	,$t9	,2
	addu	$t9	,$gp	,$t9
	sw	$s0	,0($t9)
	addiu	$s0	,$s0	,1
	li	$t9	,100
	sw	$s0	,0($gp)
	blt	$s0	,$t9	,LABEL18
	move	$t9	,$t0
	addiu	$t9	,$t9	,1
	sll	$t9	,$t9	,2
	addu	$t9	,$t9	,$gp
	lw	$s0	,0($t9)
	move	$a0	,$s0
	li	$v0	,1
	syscall		
do_while_test_end:
	lw	$ra	,4($sp)
	lw	$t0	,68($sp)
	addi	$sp	,$sp	,96
	jr	$ra	
fun3:
	addi	$sp	,$sp	,-124
	sw	$t0	,68($sp)
	sw	$t1	,64($sp)
	sw	$t2	,60($sp)
	sw	$t3	,56($sp)
	sw	$t4	,52($sp)
	sw	$t5	,48($sp)
	sw	$t6	,44($sp)
	sw	$t7	,40($sp)
	sw	$ra	,4($sp)
	move	$s0	,$a0
	move	$s1	,$a1
	move	$t7	,$a2
	move	$t6	,$a3
	lw	$t4	,104($sp)
	move	$a0	,$t4
	li	$v0	,11
	syscall		
	lw	$t3	,100($sp)
	move	$a0	,$t3
	li	$v0	,11
	syscall		
	lw	$t2	,96($sp)
	move	$a0	,$t2
	li	$v0	,11
	syscall		
	lw	$t1	,92($sp)
	move	$a0	,$t1
	li	$v0	,11
	syscall		
	lw	$t0	,88($sp)
	move	$a0	,$t0
	li	$v0	,11
	syscall		
	addu	$s2	,$s0	,$s1
	addu	$s3	,$s2	,$t7
	addu	$s4	,$s3	,$t6
	lw	$t5	,108($sp)
	addu	$s5	,$s4	,$t5
	move	$a0	,$s5
	li	$v0	,1
	syscall		
	move	$v0	,$t4
fun3_end:
	lw	$ra	,4($sp)
	lw	$t7	,40($sp)
	lw	$t6	,44($sp)
	lw	$t5	,48($sp)
	lw	$t4	,52($sp)
	lw	$t3	,56($sp)
	lw	$t2	,60($sp)
	lw	$t1	,64($sp)
	lw	$t0	,68($sp)
	addi	$sp	,$sp	,124
	jr	$ra	
main:
	addi	$sp	,$sp	,-88
	li	$v0	,5
	syscall		
	move	$s0	,$v0
	li	$v0	,5
	syscall		
	move	$t1	,$v0
	move	$s1	,$s0
	li	$t9	,1
	sw	$s0	,404($gp)
	sw	$s1	,80($sp)
	bne	$s1	,$t9	,LABEL20
	move	$a0	,$t1
	jal	do_while_test	
	j	main_end	
LABEL20:
	lw	$s0	,80($sp)
	li	$t9	,2
	sw	$s0	,80($sp)
	bne	$s0	,$t9	,LABEL21
	li	$v0	,12
	syscall		
	move	$t0	,$v0
	move	$a0	,$t0
	jal	fun1	
	jal	fun2	
	j	main_end	
LABEL21:
	lw	$s0	,80($sp)
	li	$t9	,3
	sw	$s0	,80($sp)
	bne	$s0	,$t9	,LABEL22
	li	$v0	,12
	syscall		
	move	$t0	,$v0
	li	$t9	,42
	bne	$t0	,$t9	,LABEL24
	jal	none1	
	j	main_end	
LABEL24:
	li	$t9	,47
	bne	$t0	,$t9	,LABEL25
	jal	none2	
	j	main_end	
LABEL25:
	la	$a0	,$String6
	li	$v0	,4
	syscall		
LABEL23:
	j	main_end	
LABEL22:
	lw	$s0	,80($sp)
	li	$t9	,4
	sw	$s0	,80($sp)
	bne	$s0	,$t9	,LABEL26
	jal	fun0	
	move	$s0	,$v0
	move	$a0	,$s0
	li	$v0	,11
	syscall		
	j	main_end	
LABEL26:
	lw	$s0	,80($sp)
	li	$t9	,5
	bne	$s0	,$t9	,LABEL27
	li	$t9	,1
	move	$a0	,$t9
	li	$t9	,2
	move	$a1	,$t9
	li	$t9	,3
	move	$a2	,$t9
	li	$t9	,4
	move	$a3	,$t9
	li	$t9	,5
	sw	$t9	,-16($sp)
	li	$t9	,97
	sw	$t9	,-20($sp)
	li	$t9	,98
	sw	$t9	,-24($sp)
	li	$t9	,99
	sw	$t9	,-28($sp)
	li	$t9	,100
	sw	$t9	,-32($sp)
	li	$t9	,101
	sw	$t9	,-36($sp)
	jal	fun3	
	move	$s0	,$v0
	move	$a0	,$s0
	li	$v0	,11
	syscall		
	j	main_end	
LABEL27:
	la	$a0	,$String7
	li	$v0	,4
	syscall		
main_end:
	li	$v0	,10
	syscall		
