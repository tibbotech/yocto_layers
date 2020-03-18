#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/clk.h>
#include <linux/sched_clock.h>
#include <asm/mach/time.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include "sp_stc.h"

#define SP_STC_AV2_FREQ	(13500000)		/* 13.5 MHz */

static void __iomem *sp_timer_base;

enum {
	SP_TIMER_AV2_TIMER0,
	SP_TIMER_AV2_TIMER1,
	SP_TIMER_AV2_TIMER2,
	SP_TIMER_AV2_TIMER3,
	SP_TIMER_NR_IRQS,
};
static int sp_timer_irqs[SP_TIMER_NR_IRQS];

/***************************
 * Timer3
 ***************************/

static void sp_stc_av2_timer3_start(struct clock_event_device *evt, unsigned long cycles, int periodic)
{
	stc_avReg_t *pstc_avReg = (stc_avReg_t *)sp_timer_base;
	u32 div;

	/* printk(KERN_INFO "%s: mode: %d, cycles = %u\n", __func__, (int)(evt->mode), (u32)(cycles)); */

	pstc_avReg->timer3_ctrl = 0;

	if (!periodic) {
		/*
		 * timer3_cnt only has 16 bits.
		 * When (cycles > (1 << 16)), timer3_divisor is required.
		 * => the precision is lower.
		 */
		div = (cycles >> 16) + 1;
		pstc_avReg->timer3_divisor = div - 1;
		/* a little larger than setting, make sure the ISR will see tasks' timeout as expected */
		pstc_avReg->timer3_reload = ((u32)(cycles) + div) / div - 1;

		pstc_avReg->timer3_cnt = pstc_avReg->timer3_reload;
		pstc_avReg->timer3_ctrl = SP_STC_AV_TIMER23_CTL_SRC_STC | SP_STC_AV_TIMER23_CTL_GO;
	} else { //if (evt->mode == CLOCK_EVT_MODE_PERIODIC) {
#if (SP_STC_AV2_FREQ == 1000000)
		pstc_avReg->timer3_divisor = 0;	/* CLKthis = CLKstc*/
		pstc_avReg->timer3_reload = SP_STC_AV2_FREQ/HZ - 1;
#elif (SP_STC_AV2_FREQ == 13500000)
		pstc_avReg->timer3_divisor = 3;	/* CLKthis = CLKstc / 4 */
		pstc_avReg->timer3_reload = (SP_STC_AV2_FREQ / HZ / 4) - 1;
#elif (SP_STC_AV2_FREQ == 270000000)
		pstc_avReg->timer3_divisor = 1000 - 1;	/* CLKthis = CLKstc / 1000 */
		pstc_avReg->timer3_reload = ((SP_STC_AV2_FREQ / 1000) / HZ) - 1;
#else
#error Unexpected STC frequency
#endif
		pstc_avReg->timer3_cnt = pstc_avReg->timer3_reload;
		pstc_avReg->timer3_ctrl = SP_STC_AV_TIMER23_CTL_SRC_STC | SP_STC_AV_TIMER23_CTL_RPT | SP_STC_AV_TIMER23_CTL_GO;
	}
}

static int sp_stc_av2_timer3_event_set_next_event(unsigned long cycles, struct clock_event_device *evt)
{
	sp_stc_av2_timer3_start(evt, cycles, 0);
	return 0;
}

static int sp_stc_av2_timer3_set_oneshot(struct clock_event_device *evt)
{
	sp_stc_av2_timer3_start(evt, ~0, 0);	/* max. cycle, should be updated by .set_next_event() */
	return 0;
}

static int sp_stc_av2_timer3_set_period(struct clock_event_device *evt)
{
	sp_stc_av2_timer3_start(evt, ~0, 1);	/* max. cycle, should be updated by .set_next_event() */
	return 0;
}

static int sp_stc_av2_timer3_shutdown(struct clock_event_device *clkevt)
{
	stc_avReg_t *pstc_avReg = (stc_avReg_t *)sp_timer_base;
	pstc_avReg->timer3_ctrl = 0;
	return 0;
}

static struct clock_event_device sp_clockevent_dev_av2_timer3 = {
	.name		= "sp_stc_av2_timer3_event",
	.features	= CLOCK_EVT_FEAT_ONESHOT | CLOCK_EVT_FEAT_PERIODIC,
	.rating		= 350,
	.set_next_event	= sp_stc_av2_timer3_event_set_next_event,
	.set_state_periodic = sp_stc_av2_timer3_set_period,
	.set_state_oneshot = sp_stc_av2_timer3_set_oneshot,
	.set_state_shutdown = sp_stc_av2_timer3_shutdown,
};

static irqreturn_t sp_stc_av2_timer3_isr(int irq, void *dev_id)
{
	struct clock_event_device *evt = &sp_clockevent_dev_av2_timer3;

	evt->event_handler(evt);

	return IRQ_HANDLED;
}

static struct irqaction sp_stc_av2_timer3_irq = {
	.name		= "sp_stc_av2_timer3",
	.flags		= IRQF_TIMER | IRQF_IRQPOLL | IRQF_TRIGGER_RISING,
	.handler	= sp_stc_av2_timer3_isr,
	.dev_id		= &sp_clockevent_dev_av2_timer3,
};

static void sp_clockevent_init(void)
{
	int ret;
	stc_avReg_t *pstc_avReg = (stc_avReg_t *)sp_timer_base;
	int bc_late = 0;

	pstc_avReg->timer3_ctrl = 0;

	sp_stc_av2_timer3_irq.irq = sp_timer_irqs[SP_TIMER_AV2_TIMER3];
	BUG_ON(!sp_stc_av2_timer3_irq.irq);

	sp_clockevent_dev_av2_timer3.irq = sp_stc_av2_timer3_irq.irq;
	if (irq_can_set_affinity(sp_clockevent_dev_av2_timer3.irq)) {
		sp_clockevent_dev_av2_timer3.cpumask = cpu_possible_mask;
	} else {
		sp_clockevent_dev_av2_timer3.cpumask = cpumask_of(0);
		bc_late = 1;
	}

#if (SP_STC_AV2_FREQ == 1000000)
	clockevents_config_and_register(&sp_clockevent_dev_av2_timer3, SP_STC_AV2_FREQ, 1, ((1 << 16) - 1));
#elif (SP_STC_AV2_FREQ == 13500000)
	/* 28 bits > 16 bits counter => timer3_divisor is required. */
	clockevents_config_and_register(&sp_clockevent_dev_av2_timer3, SP_STC_AV2_FREQ, 1, ((1 << 28) - 1));
#elif (SP_STC_AV2_FREQ == 270000000)
	/* 28 bits > 16 bits counter => timer3_divisor is required. */
	clockevents_config_and_register(&sp_clockevent_dev_av2_timer3, SP_STC_AV2_FREQ, 1, ((1 << 28) - 1));
#else
#error Unexpected STC frequency
#endif

	if (bc_late)
		sp_clockevent_dev_av2_timer3.cpumask = cpu_possible_mask;

	ret = setup_irq(sp_stc_av2_timer3_irq.irq, &sp_stc_av2_timer3_irq);
	if (ret) {
		printk(KERN_ERR "%s, %d: reg = %d\n", __FILE__, __LINE__, ret);
		BUG();
	}
}

u64 sp_read_sched_clock(void)
{
	stc_avReg_t *pstc_avReg = (stc_avReg_t *)sp_timer_base;

	pstc_avReg->stcl_2 = 0;
	wmb();		/* Let the STC register accesses be in-order */

	return ((pstc_avReg->stcl_0) | ((pstc_avReg->stcl_1) << 16));
}

u64 sp_clocksource_hz_read(struct clocksource *cs)
{
	return sp_read_sched_clock();
}

static void sp_clocksource_hw_init(void)
{
	stc_avReg_t *pstc_avReg = (stc_avReg_t *)sp_timer_base;
	//u32 system_clk = 0; // Read freq from DT if needed

	/*
	 * STC_AV2 (Group 99):
	 * STC: clock source for scheduler and jiffies (HZ)
	 * timer0/1: Unused.
	 * timer2: event for jiffies (if ENABLE_SP_STC_AV2_TIMER2 defined)
	 * timer3: event for jiffies/hrtimer
	 */
#if (SP_STC_AV2_FREQ == 1000000)
	pstc_avReg->stc_divisor = (system_clk / SP_STC_AV2_FREQ) - 1;	/* CLKstc = 1 us */
#elif (SP_STC_AV2_FREQ == 13500000)
	pstc_avReg->stc_divisor = (1 << 15);				/* CLKstc = (1 / 13500000) s */
#elif (SP_STC_AV2_FREQ == 270000000)
	system_clk = sp_clk_get(SP_CLK_SYSSLOW);
	pstc_avReg->stc_divisor = (system_clk / SP_STC_AV2_FREQ) - 1;	/* CLKstc = CLKsys */
#else
#error Unexpected STC frequency
#endif
	pstc_avReg->stc_64 = 0;	/* reset STC and start counting*/
}

static void sp_clocksource_resume(struct clocksource *cs)
{
	sp_clocksource_hw_init();
}

struct clocksource sp_clocksource_hz = {
	.name	= "sp_clocksource_hz",
#if (SP_STC_AV2_FREQ == 1000000)
	.rating	= 250,
#elif (SP_STC_AV2_FREQ == 13500000)
	.rating	= 350,
#elif (SP_STC_AV2_FREQ == 270000000)
	.rating	= 350,
#else
#error Unexpected STC frequency
#endif
	.read	= sp_clocksource_hz_read,
	.mask	= CLOCKSOURCE_MASK(32),
	.flags	= CLOCK_SOURCE_IS_CONTINUOUS,
	.resume	= sp_clocksource_resume,
};

static void __init sp_clocksource_init(void)
{
	u64 (*func_ptr)(void);

	func_ptr = sp_read_sched_clock;
	sp_clocksource_hw_init();
	sched_clock_register(func_ptr, 32, SP_STC_AV2_FREQ);

	if (clocksource_register_hz(&sp_clocksource_hz, SP_STC_AV2_FREQ))
		panic("%s: can't register clocksource\n", sp_clocksource_hz.name);
}

void __init sp_timer_init(void)
{
	sp_clocksource_init();
	sp_clockevent_init();
}

static int __init sp_timer_init_of(struct device_node *np)
{
	u32 nr_irqs, i;

	nr_irqs = of_irq_count(np);

	for (i = SP_TIMER_AV2_TIMER0; i < nr_irqs; i++) {
		sp_timer_irqs[i] = irq_of_parse_and_map(np, i);
		WARN(!sp_timer_irqs[i], "%s: no IRQ for timer%d", __func__, i);
	}

	sp_timer_base = of_iomap(np, 0);
	if (!sp_timer_base)
		panic("%s: Could not ioremap system timer base\n", __func__);

	sp_clocksource_init();
	sp_clockevent_init();

	return 0;
}

CLOCKSOURCE_OF_DECLARE(sp_timer, "sunplus,sp-stc", sp_timer_init_of);
