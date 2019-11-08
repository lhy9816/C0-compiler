.data
	$String0:.asciiz "BAD RESULT IN fib"
	$String1:.asciiz "FUNC WITH PARA BUT NO RETURN"
	$String2:.asciiz ""
	$String3:.asciiz " !#$%&'()*+,-./0123456789:;<=>?@ ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
	$String4:.asciiz "WRONG INPUT IN CASE3"
	$String5:.asciiz "WRONG INPUT!"
.text
	add	$fp	,$sp	,$zero
	j	main	
fib:
	addi	$sp	,$sp	,4
	sw	$t0	,-24($sp)
	sw	$t1	,-28($sp)
	sw	$t2	,-32($sp)
	sw	$t3	,-36($sp)
	sw	$t4	,-40($sp)
	sw	$t5	,-44($sp)
	sw	$t6	,-48($sp)
	sw	$t7	,-52($sp)
	sw	$s0	,-56($sp)
	sw	$s1	,-60($sp)
	sw	$s2	,-64($sp)
	sw	$s3	,-68($sp)
	sw	$s4	,-72($sp)
	sw	$s5	,-76($sp)
	sw	$s6	,-80($sp)
	sw	$s7	,-84($sp)
	sw	$fp	,-88($sp)
	sw	$ra	,-92($sp)
	move	$fp	,$sp
	addi	$sp	,$sp	,-96
	li	$t9	,1
	beq	$t9	,$zero	,LABEL0
	li	$t9	,1
	lw	$t0	,0($fp)
	bne	$t0	,$t9	,LABEL1
	li	$v0	,1
	j	fib_end	
	j	LABEL2	
LABEL1:
	li	$t9	,2
	bne	$t0	,$t9	,LABEL3
	li	$v0	,2
	j	fib_end	
	j	LABEL4	
LABEL3:
	sub	$t1	,$t0	,1
	move	$a0	,$t1
	sw	$t1	,0($sp)
	addi	$sp	,$sp	,-4
	jal	fib	
	move	$t2	,$v0
	sub	$t4	,$t0	,2
	move	$a1	,$t4
	sw	$t4	,0($sp)
	addi	$sp	,$sp	,-4
	jal	fib	
	move	$t5	,$v0
	add	$t6	,$t2	,$t5
	move	$v0	,$t6
	j	fib_end	
LABEL4:
LABEL2:
	j	LABEL5	
LABEL0:
	la	$a0	,$String0
	li	$v0	,4
	syscall		
	li	$v0	,-1
	j	fib_end	
LABEL5:
fib_end:
	lw	$ra	,4($sp)
	lw	$fp	,8($sp)
	lw	$s7	,12($sp)
	lw	$s6	,16($sp)
	lw	$s5	,20($sp)
	lw	$s4	,24($sp)
	lw	$s3	,28($sp)
	lw	$s2	,32($sp)
	lw	$s1	,36($sp)
	lw	$s0	,40($sp)
	lw	$t7	,44($sp)
	lw	$t6	,48($sp)
	lw	$t5	,52($sp)
	lw	$t4	,56($sp)
	lw	$t3	,60($sp)
	lw	$t2	,64($sp)
	lw	$t1	,68($sp)
	lw	$t0	,72($sp)
	addi	$sp	,$sp	,96
	jr	$ra	
fun0:
	addi	$sp	,$sp	,0
	sw	$t0	,-16($sp)
	sw	$t1	,-20($sp)
	sw	$t2	,-24($sp)
	sw	$t3	,-28($sp)
	sw	$t4	,-32($sp)
	sw	$t5	,-36($sp)
	sw	$t6	,-40($sp)
	sw	$t7	,-44($sp)
	sw	$s0	,-48($sp)
	sw	$s1	,-52($sp)
	sw	$s2	,-56($sp)
	sw	$s3	,-60($sp)
	sw	$s4	,-64($sp)
	sw	$s5	,-68($sp)
	sw	$s6	,-72($sp)
	sw	$s7	,-76($sp)
	sw	$fp	,-80($sp)
	sw	$ra	,-84($sp)
	move	$fp	,$sp
	addi	$sp	,$sp	,-88
	li	$t9	,8
	sub	$t9	,$fp	,$t9
	li	$t8	,43
	sw	$t8	,0($t9)
	li	$t9	,12
	sub	$t9	,$fp	,$t9
	li	$t8	,45
	sw	$t8	,0($t9)
	li	$t9	,16
	sub	$t9	,$fp	,$t9
	li	$t8	,42
	sw	$t8	,0($t9)
	li	$t9	,20
	sub	$t9	,$fp	,$t9
	li	$t8	,47
	sw	$t8	,0($t9)
	li	$t9	,24
	sub	$t9	,$fp	,$t9
	li	$t8	,95
	sw	$t8	,0($t9)
	li	$t9	,28
	sub	$t9	,$fp	,$t9
	li	$t8	,48
	sw	$t8	,0($t9)
	li	$t9	,32
	sub	$t9	,$fp	,$t9
	li	$t8	,49
	sw	$t8	,0($t9)
	li	$t9	,36
	sub	$t9	,$fp	,$t9
	li	$t8	,50
	sw	$t8	,0($t9)
	li	$t9	,40
	sub	$t9	,$fp	,$t9
	li	$t8	,51
	sw	$t8	,0($t9)
	li	$t9	,44
	sub	$t9	,$fp	,$t9
	li	$t8	,52
	sw	$t8	,0($t9)
	li	$t9	,48
	sub	$t9	,$fp	,$t9
	li	$t8	,53
	sw	$t8	,0($t9)
	li	$t9	,52
	sub	$t9	,$fp	,$t9
	li	$t8	,54
	sw	$t8	,0($t9)
	li	$t9	,56
	sub	$t9	,$fp	,$t9
	li	$t8	,55
	sw	$t8	,0($t9)
	li	$t9	,60
	sub	$t9	,$fp	,$t9
	li	$t8	,56
	sw	$t8	,0($t9)
	li	$t9	,64
	sub	$t9	,$fp	,$t9
	li	$t8	,57
	sw	$t8	,0($t9)
	li	$t9	,68
	sub	$t9	,$fp	,$t9
	li	$t8	,97
	sw	$t8	,0($t9)
	li	$t9	,72
	sub	$t9	,$fp	,$t9
	li	$t8	,98
	sw	$t8	,0($t9)
	li	$t9	,76
	sub	$t9	,$fp	,$t9
	li	$t8	,99
	sw	$t8	,0($t9)
	li	$t9	,80
	sub	$t9	,$fp	,$t9
	li	$t8	,100
	sw	$t8	,0($t9)
	li	$t9	,84
	sub	$t9	,$fp	,$t9
	li	$t8	,101
	sw	$t8	,0($t9)
	li	$t9	,88
	sub	$t9	,$fp	,$t9
	li	$t8	,102
	sw	$t8	,0($t9)
	li	$t9	,92
	sub	$t9	,$fp	,$t9
	li	$t8	,103
	sw	$t8	,0($t9)
	li	$t9	,96
	sub	$t9	,$fp	,$t9
	li	$t8	,104
	sw	$t8	,0($t9)
	li	$t9	,100
	sub	$t9	,$fp	,$t9
	li	$t8	,105
	sw	$t8	,0($t9)
	li	$t9	,104
	sub	$t9	,$fp	,$t9
	li	$t8	,106
	sw	$t8	,0($t9)
	li	$t9	,108
	sub	$t9	,$fp	,$t9
	li	$t8	,107
	sw	$t8	,0($t9)
	li	$t9	,112
	sub	$t9	,$fp	,$t9
	li	$t8	,108
	sw	$t8	,0($t9)
	li	$t9	,116
	sub	$t9	,$fp	,$t9
	li	$t8	,109
	sw	$t8	,0($t9)
	li	$t9	,120
	sub	$t9	,$fp	,$t9
	li	$t8	,110
	sw	$t8	,0($t9)
	li	$t9	,124
	sub	$t9	,$fp	,$t9
	li	$t8	,111
	sw	$t8	,0($t9)
	li	$t9	,128
	sub	$t9	,$fp	,$t9
	li	$t8	,112
	sw	$t8	,0($t9)
	li	$t9	,132
	sub	$t9	,$fp	,$t9
	li	$t8	,113
	sw	$t8	,0($t9)
	li	$t9	,136
	sub	$t9	,$fp	,$t9
	li	$t8	,114
	sw	$t8	,0($t9)
	li	$t9	,140
	sub	$t9	,$fp	,$t9
	li	$t8	,115
	sw	$t8	,0($t9)
	li	$t9	,144
	sub	$t9	,$fp	,$t9
	li	$t8	,116
	sw	$t8	,0($t9)
	li	$t9	,148
	sub	$t9	,$fp	,$t9
	li	$t8	,117
	sw	$t8	,0($t9)
	li	$t9	,152
	sub	$t9	,$fp	,$t9
	li	$t8	,118
	sw	$t8	,0($t9)
	li	$t9	,156
	sub	$t9	,$fp	,$t9
	li	$t8	,119
	sw	$t8	,0($t9)
	li	$t9	,160
	sub	$t9	,$fp	,$t9
	li	$t8	,120
	sw	$t8	,0($t9)
	li	$t9	,164
	sub	$t9	,$fp	,$t9
	li	$t8	,121
	sw	$t8	,0($t9)
	li	$t9	,168
	sub	$t9	,$fp	,$t9
	li	$t8	,122
	sw	$t8	,0($t9)
	li	$t9	,172
	sub	$t9	,$fp	,$t9
	li	$t8	,65
	sw	$t8	,0($t9)
	li	$t9	,176
	sub	$t9	,$fp	,$t9
	li	$t8	,66
	sw	$t8	,0($t9)
	li	$t9	,180
	sub	$t9	,$fp	,$t9
	li	$t8	,67
	sw	$t8	,0($t9)
	li	$t9	,184
	sub	$t9	,$fp	,$t9
	li	$t8	,68
	sw	$t8	,0($t9)
	li	$t9	,188
	sub	$t9	,$fp	,$t9
	li	$t8	,69
	sw	$t8	,0($t9)
	li	$t9	,192
	sub	$t9	,$fp	,$t9
	li	$t8	,70
	sw	$t8	,0($t9)
	li	$t9	,196
	sub	$t9	,$fp	,$t9
	li	$t8	,71
	sw	$t8	,0($t9)
	li	$t9	,200
	sub	$t9	,$fp	,$t9
	li	$t8	,72
	sw	$t8	,0($t9)
	li	$t9	,204
	sub	$t9	,$fp	,$t9
	li	$t8	,73
	sw	$t8	,0($t9)
	li	$t9	,208
	sub	$t9	,$fp	,$t9
	li	$t8	,74
	sw	$t8	,0($t9)
	li	$t9	,212
	sub	$t9	,$fp	,$t9
	li	$t8	,75
	sw	$t8	,0($t9)
	li	$t9	,216
	sub	$t9	,$fp	,$t9
	li	$t8	,76
	sw	$t8	,0($t9)
	li	$t9	,220
	sub	$t9	,$fp	,$t9
	li	$t8	,77
	sw	$t8	,0($t9)
	li	$t9	,224
	sub	$t9	,$fp	,$t9
	li	$t8	,78
	sw	$t8	,0($t9)
	li	$t9	,228
	sub	$t9	,$fp	,$t9
	li	$t8	,79
	sw	$t8	,0($t9)
	li	$t9	,232
	sub	$t9	,$fp	,$t9
	li	$t8	,80
	sw	$t8	,0($t9)
	li	$t9	,236
	sub	$t9	,$fp	,$t9
	li	$t8	,81
	sw	$t8	,0($t9)
	li	$t9	,240
	sub	$t9	,$fp	,$t9
	li	$t8	,82
	sw	$t8	,0($t9)
	li	$t9	,244
	sub	$t9	,$fp	,$t9
	li	$t8	,83
	sw	$t8	,0($t9)
	li	$t9	,248
	sub	$t9	,$fp	,$t9
	li	$t8	,84
	sw	$t8	,0($t9)
	li	$t9	,252
	sub	$t9	,$fp	,$t9
	li	$t8	,85
	sw	$t8	,0($t9)
	li	$t9	,256
	sub	$t9	,$fp	,$t9
	li	$t8	,86
	sw	$t8	,0($t9)
	li	$t9	,260
	sub	$t9	,$fp	,$t9
	li	$t8	,87
	sw	$t8	,0($t9)
	li	$t9	,264
	sub	$t9	,$fp	,$t9
	li	$t8	,88
	sw	$t8	,0($t9)
	li	$t9	,268
	sub	$t9	,$fp	,$t9
	li	$t8	,89
	sw	$t8	,0($t9)
	li	$t9	,272
	sub	$t9	,$fp	,$t9
	li	$t8	,90
	sw	$t8	,0($t9)
	li	$a0	,95
	li	$v0	,11
	syscall		
	li	$t9	,10
	addi	$t9	,$t9	,2
	sll	$t9	,$t9	,2
	sub	$t9	,$fp	,$t9
	lw	$t0	,0($t9)
	move	$v0	,$t0
	j	fun0_end	
fun0_end:
	lw	$ra	,4($sp)
	lw	$fp	,8($sp)
	lw	$s7	,12($sp)
	lw	$s6	,16($sp)
	lw	$s5	,20($sp)
	lw	$s4	,24($sp)
	lw	$s3	,28($sp)
	lw	$s2	,32($sp)
	lw	$s1	,36($sp)
	lw	$s0	,40($sp)
	lw	$t7	,44($sp)
	lw	$t6	,48($sp)
	lw	$t5	,52($sp)
	lw	$t4	,56($sp)
	lw	$t3	,60($sp)
	lw	$t2	,64($sp)
	lw	$t1	,68($sp)
	lw	$t0	,72($sp)
	addi	$sp	,$sp	,88
	jr	$ra	
fun1:
	addi	$sp	,$sp	,4
	sw	$t0	,-56($sp)
	sw	$t1	,-60($sp)
	sw	$t2	,-64($sp)
	sw	$t3	,-68($sp)
	sw	$t4	,-72($sp)
	sw	$t5	,-76($sp)
	sw	$t6	,-80($sp)
	sw	$t7	,-84($sp)
	sw	$s0	,-88($sp)
	sw	$s1	,-92($sp)
	sw	$s2	,-96($sp)
	sw	$s3	,-100($sp)
	sw	$s4	,-104($sp)
	sw	$s5	,-108($sp)
	sw	$s6	,-112($sp)
	sw	$s7	,-116($sp)
	sw	$fp	,-120($sp)
	sw	$ra	,-124($sp)
	move	$fp	,$sp
	addi	$sp	,$sp	,-128
	li	$t9	,4
	add	$t9	,$gp	,$t9
	li	$t8	,1
	sw	$t8	,0($t9)
	li	$t9	,8
	add	$t9	,$gp	,$t9
	li	$t8	,-1
	sw	$t8	,0($t9)
	li	$t9	,1
	bne	$t9	,$zero	,LABEL6
	li	$t0	,1
	j	LABEL7	
LABEL6:
	li	$t9	,0
	beq	$t9	,$zero	,LABEL8
	li	$t0	,2
	j	LABEL9	
LABEL8:
	li	$t9	,2
	bgt	$t9	,$zero	,LABEL10
	li	$t0	,3
	j	LABEL11	
LABEL10:
	li	$t9	,0
	addi	$t9	,$t9	,1
	sll	$t9	,$t9	,2
	add	$t9	,$t9	,$gp
	lw	$t1	,0($t9)
	li	$t9	,1
	addi	$t9	,$t9	,1
	sll	$t9	,$t9	,2
	add	$t9	,$t9	,$gp
	lw	$t2	,0($t9)
	add	$t4	,$t1	,$t2
	li	$t9	,4
	ble	$t4	,$t9	,LABEL12
	li	$t0	,4
	j	LABEL13	
LABEL12:
	li	$t9	,0
	addi	$t9	,$t9	,1
	sll	$t9	,$t9	,2
	add	$t9	,$t9	,$gp
	lw	$t5	,0($t9)
	li	$t9	,0
	div	$t6	,$t9	,$t5
	li	$t9	,0
	addi	$t9	,$t9	,1
	sll	$t9	,$t9	,2
	add	$t9	,$t9	,$gp
	lw	$t8	,0($t9)
	mul	$s1	,$t8	,10
	li	$t9	,0
	addi	$t9	,$t9	,1
	sll	$t9	,$t9	,2
	add	$t9	,$t9	,$gp
	lw	$s5	,0($t9)
	add	$s6	,$s5	,4
	move	$a0	,$s6
	sw	$s6	,0($sp)
	addi	$sp	,$sp	,-4
	jal	fib	
	move	$s7	,$v0
	sw	$t0	,-4($fp)
	li	$t0	,0
	add	$t0	,$s1	,$s7
	blt	$t6	,$t0	,LABEL14
	sw	$t0	,-48($fp)
	li	$t0	,0
	li	$t0	,5
	j	LABEL15	
LABEL14:
	li	$t0	,6
LABEL15:
LABEL13:
LABEL11:
LABEL9:
LABEL7:
	la	$a0	,$String1
	li	$v0	,4
	syscall		
	sw	$t0	,-4($fp)
	li	$t0	,0
	sw	$t0	,-52($fp)
	li	$t0	,0
	sw	$t0	,-4($fp)
	li	$t0	,0
	lw	$t0	,0($fp)
	lw	$t0	,-4($fp)
	add	$t0	,$t0	,$t0
	sw	$t0	,0($fp)
	li	$t0	,0
	lw	$a0	,-52($fp)
	li	$v0	,1
	syscall		
fun1_end:
	lw	$ra	,4($sp)
	lw	$fp	,8($sp)
	lw	$s7	,12($sp)
	lw	$s6	,16($sp)
	lw	$s5	,20($sp)
	lw	$s4	,24($sp)
	lw	$s3	,28($sp)
	lw	$s2	,32($sp)
	lw	$s1	,36($sp)
	lw	$s0	,40($sp)
	lw	$t7	,44($sp)
	lw	$t6	,48($sp)
	lw	$t5	,52($sp)
	lw	$t4	,56($sp)
	lw	$t3	,60($sp)
	lw	$t2	,64($sp)
	lw	$t1	,68($sp)
	lw	$t0	,72($sp)
	addi	$sp	,$sp	,128
	jr	$ra	
fun2:
	addi	$sp	,$sp	,0
	sw	$t0	,-40($sp)
	sw	$t1	,-44($sp)
	sw	$t2	,-48($sp)
	sw	$t3	,-52($sp)
	sw	$t4	,-56($sp)
	sw	$t5	,-60($sp)
	sw	$t6	,-64($sp)
	sw	$t7	,-68($sp)
	sw	$s0	,-72($sp)
	sw	$s1	,-76($sp)
	sw	$s2	,-80($sp)
	sw	$s3	,-84($sp)
	sw	$s4	,-88($sp)
	sw	$s5	,-92($sp)
	sw	$s6	,-96($sp)
	sw	$s7	,-100($sp)
	sw	$fp	,-104($sp)
	sw	$ra	,-108($sp)
	move	$fp	,$sp
	addi	$sp	,$sp	,-112
	li	$t0	,2
	addi	$t9	,$t0	,1
	sll	$t9	,$t9	,2
	sub	$t9	,$fp	,$t9
	sw	$t0	,0($t9)
	li	$t9	,3
	move	$a0	,$t9
	sw	$t9	,0($sp)
	addi	$sp	,$sp	,-4
	jal	fib	
	move	$t1	,$v0
	li	$t9	,54
	sub	$t2	,$t9	,$t1
	lw	$t0	,0($fp)
	addi	$t0	,$t0	,1
	sll	$t0	,$t0	,2
	sub	$t0	,$fp	,$t0
	lw	$t4	,0($t0)
	div	$t5	,$t0	,$t4
	add	$t6	,$t5	,1
	mul	$t8	,$t2	,$t6
	li	$t9	,0
	sub	$t8	,$t9	,$t8
	move	$s1	,$t8
	move	$a0	,$s1
	li	$v0	,1
	syscall		
	li	$s1	,966
	move	$a0	,$s1
	li	$v0	,1
	syscall		
	la	$a0	,$String2
	li	$v0	,4
	syscall		
	la	$a0	,$String3
	li	$v0	,4
	syscall		
	j	fun2_end	
fun2_end:
	lw	$ra	,4($sp)
	lw	$fp	,8($sp)
	lw	$s7	,12($sp)
	lw	$s6	,16($sp)
	lw	$s5	,20($sp)
	lw	$s4	,24($sp)
	lw	$s3	,28($sp)
	lw	$s2	,32($sp)
	lw	$s1	,36($sp)
	lw	$s0	,40($sp)
	lw	$t7	,44($sp)
	lw	$t6	,48($sp)
	lw	$t5	,52($sp)
	lw	$t4	,56($sp)
	lw	$t3	,60($sp)
	lw	$t2	,64($sp)
	lw	$t1	,68($sp)
	lw	$t0	,72($sp)
	addi	$sp	,$sp	,112
	jr	$ra	
none1:
	addi	$sp	,$sp	,0
	sw	$t0	,0($sp)
	sw	$t1	,-4($sp)
	sw	$t2	,-8($sp)
	sw	$t3	,-12($sp)
	sw	$t4	,-16($sp)
	sw	$t5	,-20($sp)
	sw	$t6	,-24($sp)
	sw	$t7	,-28($sp)
	sw	$s0	,-32($sp)
	sw	$s1	,-36($sp)
	sw	$s2	,-40($sp)
	sw	$s3	,-44($sp)
	sw	$s4	,-48($sp)
	sw	$s5	,-52($sp)
	sw	$s6	,-56($sp)
	sw	$s7	,-60($sp)
	sw	$fp	,-64($sp)
	sw	$ra	,-68($sp)
	move	$fp	,$sp
	addi	$sp	,$sp	,-72
none1_end:
	lw	$ra	,4($sp)
	lw	$fp	,8($sp)
	lw	$s7	,12($sp)
	lw	$s6	,16($sp)
	lw	$s5	,20($sp)
	lw	$s4	,24($sp)
	lw	$s3	,28($sp)
	lw	$s2	,32($sp)
	lw	$s1	,36($sp)
	lw	$s0	,40($sp)
	lw	$t7	,44($sp)
	lw	$t6	,48($sp)
	lw	$t5	,52($sp)
	lw	$t4	,56($sp)
	lw	$t3	,60($sp)
	lw	$t2	,64($sp)
	lw	$t1	,68($sp)
	lw	$t0	,72($sp)
	addi	$sp	,$sp	,72
	jr	$ra	
none2:
	addi	$sp	,$sp	,0
	sw	$t0	,0($sp)
	sw	$t1	,-4($sp)
	sw	$t2	,-8($sp)
	sw	$t3	,-12($sp)
	sw	$t4	,-16($sp)
	sw	$t5	,-20($sp)
	sw	$t6	,-24($sp)
	sw	$t7	,-28($sp)
	sw	$s0	,-32($sp)
	sw	$s1	,-36($sp)
	sw	$s2	,-40($sp)
	sw	$s3	,-44($sp)
	sw	$s4	,-48($sp)
	sw	$s5	,-52($sp)
	sw	$s6	,-56($sp)
	sw	$s7	,-60($sp)
	sw	$fp	,-64($sp)
	sw	$ra	,-68($sp)
	move	$fp	,$sp
	addi	$sp	,$sp	,-72
none2_end:
	lw	$ra	,4($sp)
	lw	$fp	,8($sp)
	lw	$s7	,12($sp)
	lw	$s6	,16($sp)
	lw	$s5	,20($sp)
	lw	$s4	,24($sp)
	lw	$s3	,28($sp)
	lw	$s2	,32($sp)
	lw	$s1	,36($sp)
	lw	$s0	,40($sp)
	lw	$t7	,44($sp)
	lw	$t6	,48($sp)
	lw	$t5	,52($sp)
	lw	$t4	,56($sp)
	lw	$t3	,60($sp)
	lw	$t2	,64($sp)
	lw	$t1	,68($sp)
	lw	$t0	,72($sp)
	addi	$sp	,$sp	,72
	jr	$ra	
do_while_test:
	addi	$sp	,$sp	,4
	sw	$t0	,-12($sp)
	sw	$t1	,-16($sp)
	sw	$t2	,-20($sp)
	sw	$t3	,-24($sp)
	sw	$t4	,-28($sp)
	sw	$t5	,-32($sp)
	sw	$t6	,-36($sp)
	sw	$t7	,-40($sp)
	sw	$s0	,-44($sp)
	sw	$s1	,-48($sp)
	sw	$s2	,-52($sp)
	sw	$s3	,-56($sp)
	sw	$s4	,-60($sp)
	sw	$s5	,-64($sp)
	sw	$s6	,-68($sp)
	sw	$s7	,-72($sp)
	sw	$fp	,-76($sp)
	sw	$ra	,-80($sp)
	move	$fp	,$sp
	addi	$sp	,$sp	,-84
	li	$t0	,0
LABEL16:
	addi	$t9	,$t0	,1
	sll	$t9	,$t9	,2
	add	$t9	,$gp	,$t9
	sw	$t0	,0($t9)
	add	$t1	,$t0	,1
	move	$t0	,$t1
	li	$t9	,100
	blt	$t0	,$t9	,LABEL16
	lw	$t2	,0($fp)
	addi	$t2	,$t2	,1
	sll	$t2	,$t2	,2
	add	$t2	,$t2	,$gp
	lw	$t4	,0($t2)
	move	$a0	,$t4
	li	$v0	,1
	syscall		
do_while_test_end:
	lw	$ra	,4($sp)
	lw	$fp	,8($sp)
	lw	$s7	,12($sp)
	lw	$s6	,16($sp)
	lw	$s5	,20($sp)
	lw	$s4	,24($sp)
	lw	$s3	,28($sp)
	lw	$s2	,32($sp)
	lw	$s1	,36($sp)
	lw	$s0	,40($sp)
	lw	$t7	,44($sp)
	lw	$t6	,48($sp)
	lw	$t5	,52($sp)
	lw	$t4	,56($sp)
	lw	$t3	,60($sp)
	lw	$t2	,64($sp)
	lw	$t1	,68($sp)
	lw	$t0	,72($sp)
	addi	$sp	,$sp	,84
	jr	$ra	
main:
	addi	$sp	,$sp	,0
	sw	$t0	,-12($sp)
	sw	$t1	,-16($sp)
	sw	$t2	,-20($sp)
	sw	$t3	,-24($sp)
	sw	$t4	,-28($sp)
	sw	$t5	,-32($sp)
	sw	$t6	,-36($sp)
	sw	$t7	,-40($sp)
	sw	$s0	,-44($sp)
	sw	$s1	,-48($sp)
	sw	$s2	,-52($sp)
	sw	$s3	,-56($sp)
	sw	$s4	,-60($sp)
	sw	$s5	,-64($sp)
	sw	$s6	,-68($sp)
	sw	$s7	,-72($sp)
	sw	$fp	,-76($sp)
	sw	$ra	,-80($sp)
	move	$fp	,$sp
	addi	$sp	,$sp	,-84
	li	$v0	,5
	syscall		
	move	$t0	,$v0
	li	$v0	,5
	syscall		
	move	$t1	,$v0
	li	$t9	,1
	bne	$t0	,$t9	,LABEL18
	li	$v0	,5
	syscall		
	move	$t1	,$v0
	move	$a0	,$t1
	sw	$t1	,0($sp)
	addi	$sp	,$sp	,-4
	jal	do_while_test	
	j	LABEL17	
LABEL18:
	li	$t9	,2
	bne	$t0	,$t9	,LABEL19
	li	$v0	,12
	syscall		
	move	$t2	,$v0
	move	$a1	,$t2
	sw	$t2	,0($sp)
	addi	$sp	,$sp	,-4
	jal	fun1	
	jal	fun2	
	j	LABEL17	
LABEL19:
	li	$t9	,3
	bne	$t0	,$t9	,LABEL20
	li	$v0	,12
	syscall		
	move	$t2	,$v0
	li	$t9	,42
	bne	$t2	,$t9	,LABEL22
	jal	none1	
	j	LABEL21	
LABEL22:
	li	$t9	,47
	bne	$t2	,$t9	,LABEL23
	jal	none2	
	j	LABEL21	
LABEL23:
	la	$a0	,$String4
	li	$v0	,4
	syscall		
LABEL21:
	j	LABEL17	
LABEL20:
	li	$t9	,4
	bne	$t0	,$t9	,LABEL24
	jal	fun0	
	move	$t4	,$v0
	move	$a0	,$t4
	li	$v0	,11
	syscall		
	j	LABEL17	
LABEL24:
	la	$a0	,$String5
	li	$v0	,4
	syscall		
LABEL17:
	li	$a0	,2
	li	$v0	,1
	syscall		
main_end:
	li	$v0	,10
	syscall		
