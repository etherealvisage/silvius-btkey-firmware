.section .reset
.global _start

_start:
	/* is this a NMI (i.e. watchdog timer reset)? */
	/* grab NMI bit from the Status register */
	mfc0	$k0, $12
	ext	$k0, $k0, 19, 1
	beqz	$k0, .no_nmi
	nop

	la	$k0, nmi_handler
	jr 	$k0
	nop

.no_nmi:
	/* it's a proper reset. */
	/* begin setup by initializing RAM */
	jal	initialize_ram

	/* continue by jumping into target code */
.extern entry
	jal	entry

.loop_forever:
	j	.loop_forever
	nop

initialize_ram:
.extern _gp
	/* set up $gp register */
	la 	$gp, _gp
	/* set up $sp register */
	la	$sp, _sp

	/* set up bss */
.extern bss_ram_begin
.extern bss_ram_end
	la	$t0, bss_ram_begin
	la	$t1, bss_ram_end

.zero_loop:
	beq	$t0, $t1, .zero_loop_end
	nop

	sw	$zero, 0($t0)
	add	$t0, $t0, 4
	j	.zero_loop
	nop

.zero_loop_end:
	/* set up data */
.extern data_ram_begin
.extern data_ram_end
.extern data_flash_end
	la	$t0, data_ram_begin
	la	$t1, data_ram_end
	la	$t2, data_flash_begin

.copy_loop:
	beq	$t0, $t1, .copy_loop_end
	nop

	lw	$t3, 0($t2)
	sw	$t3, 0($t0)

	add	$t0, $t0, 4
	add	$t2, $t2, 4

	j	.copy_loop
	nop
.copy_loop_end:

	jr 	$ra
	nop
