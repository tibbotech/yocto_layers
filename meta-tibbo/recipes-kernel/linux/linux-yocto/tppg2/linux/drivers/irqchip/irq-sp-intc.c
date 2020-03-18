#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <asm/io.h>
#include <asm/exception.h>
#include <asm/mach/irq.h>
#include <linux/irqchip.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#define dprn(...)            /* empty */
//#define dprn(fmt, arg...)  early_printk(fmt, ##arg)

#define SP_INTC_HWIRQ_MIN     0
#define SP_INTC_HWIRQ_MAX     223

struct intG0Reg_st {
	/* Interrupt G0 */
	volatile unsigned int intr_type[7];
	volatile unsigned int intr_polarity[7];
	volatile unsigned int priority[7];
	volatile unsigned int intr_mask[7];
	volatile unsigned int g15_reserved_0[4];
};

struct intG1Reg_st {
	/* Interrupt G1 */
	volatile unsigned int intr_clr[7];
	volatile unsigned int masked_fiqs[7];
	volatile unsigned int masked_irqs[7];
	volatile unsigned int g21_reserved_0[10];
	volatile unsigned int intr_group;
};

static struct sp_intctl {
	__iomem struct intG0Reg_st *g0;
	__iomem struct intG1Reg_st *g1;
	 int hwirq_start;
	 int hwirq_end;   /* exclude */
	 struct irq_domain *domain;
} sp_intc;


static void sp_intc_ack_irq(struct irq_data *data);
static void sp_intc_mask_irq(struct irq_data *data);
static void sp_intc_unmask_irq(struct irq_data *data);
static int sp_intc_set_type(struct irq_data *data, unsigned int flow_type);

static struct irq_chip sp_intc_chip = {
	.name = "sp_intc",
	.irq_ack = sp_intc_ack_irq,
	.irq_mask = sp_intc_mask_irq,
	.irq_unmask = sp_intc_unmask_irq,
	.irq_set_type = sp_intc_set_type,
};

static void sp_intc_ack_irq(struct irq_data *data)
{
	u32 idx, mask;

	dprn("%s: hwirq=%lu\n", __func__, data->hwirq);

	if ((data->hwirq < sp_intc.hwirq_start) || (data->hwirq >= sp_intc.hwirq_end))
		return;

	idx = data->hwirq / 32;
	mask = (1 << (data->hwirq % 32));

	writel(mask, &sp_intc.g1->intr_clr[idx]);
}

static void sp_intc_mask_irq(struct irq_data *data)
{
	u32 idx, mask;

	dprn("%s: hwirq=%lu\n", __func__, data->hwirq);

	if ((data->hwirq < sp_intc.hwirq_start) || (data->hwirq >= sp_intc.hwirq_end))
		return;

	idx = data->hwirq / 32;
	mask = readl(&sp_intc.g0->intr_mask[idx]);
	mask &= ~(1 << (data->hwirq % 32));
	writel(mask, &sp_intc.g0->intr_mask[idx]);
}

static void sp_intc_unmask_irq(struct irq_data *data)
{
	u32 idx, mask;

	dprn("%s: hwirq=%lu\n", __func__, data->hwirq);

	if ((data->hwirq < sp_intc.hwirq_start) || (data->hwirq >= sp_intc.hwirq_end))
		return;

	idx = data->hwirq / 32;
	mask = readl(&sp_intc.g0->intr_mask[idx]);
	mask |= (1 << (data->hwirq % 32));
	writel(mask, &sp_intc.g0->intr_mask[idx]);
}

static int sp_intc_set_type(struct irq_data *data, unsigned int flow_type)
{
	u32 idx, mask;
	u32 edge_type; /* 0=level, 1=edge */
	u32 trig_lvl;  /* 0=high, 1=low */

	dprn("%s: hwirq=%lu type=%u\n", __func__, data->hwirq, flow_type);

	if ((data->hwirq < sp_intc.hwirq_start) || (data->hwirq >= sp_intc.hwirq_end))
		return -EBADR;

	/* update the chip/handler */
	if (flow_type & IRQ_TYPE_LEVEL_MASK)
		irq_set_chip_handler_name_locked(data, &sp_intc_chip,
						   handle_level_irq, NULL);
	else
		irq_set_chip_handler_name_locked(data, &sp_intc_chip,
						   handle_edge_irq, NULL);

	idx = data->hwirq / 32;
	edge_type = readl(&sp_intc.g0->intr_type[idx]);
	trig_lvl = readl(&sp_intc.g0->intr_polarity[idx]);
	mask = (1 << (data->hwirq % 32));

	switch (flow_type) {
	case IRQ_TYPE_EDGE_RISING:
		writel((edge_type | mask), &sp_intc.g0->intr_type[idx]);
		writel((trig_lvl & ~mask), &sp_intc.g0->intr_polarity[idx]);
		break;
	case IRQ_TYPE_EDGE_FALLING:
		writel((edge_type | mask), &sp_intc.g0->intr_type[idx]);
		writel((trig_lvl | mask), &sp_intc.g0->intr_polarity[idx]);
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		writel((edge_type & ~mask), &sp_intc.g0->intr_type[idx]);
		writel((trig_lvl & ~mask), &sp_intc.g0->intr_polarity[idx]);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		writel((edge_type & ~mask), &sp_intc.g0->intr_type[idx]);
		writel((trig_lvl | mask), &sp_intc.g0->intr_polarity[idx]);
		break;
	default:
		pr_err("%s: type=%d\n", __func__, flow_type);
		return -EBADR;
	}

	return IRQ_SET_MASK_OK;
}

/* return -1 if no interrupt */
static int sp_intc_get_hwirq(void)
{
	int hwirq, mask;
	int i;

	/* look up reg number i */
#if defined(CONFIG_MACH_PENTAGRAM_SC7021_ACHIP) || defined(CONFIG_MACH_PENTAGRAM_SC7021_BCHIP)
	mask = (readl(&sp_intc.g1->intr_group) >> 8) & 0x7f; /* [14:8] for irq group */
	if (mask) {
		i = fls(mask) - 1;
#else
	for (i = 0; i < 7; i++) {
#endif
		mask = readl(&sp_intc.g1->masked_irqs[i]);
		if (mask) {
			hwirq = (i << 5) + fls(mask) - 1;
			dprn(" [%d]", hwirq);
			return hwirq;
		}
	}
	return -1;
}

static void sp_intc_handle_irq_cascaded(struct irq_desc *desc)
{
        struct irq_chip *host_chip = irq_desc_get_chip(desc);
	int hwirq;

        chained_irq_enter(host_chip, desc);

	while ((hwirq = sp_intc_get_hwirq()) >= 0) {
                generic_handle_irq(irq_find_mapping(sp_intc.domain, (unsigned int)hwirq));
        }

        chained_irq_exit(host_chip, desc);
}

static int sp_intc_handle_one_round(struct pt_regs *regs)
{
	int ret = -EINVAL;
	int hwirq;

	while ((hwirq = sp_intc_get_hwirq()) >= 0) {
		handle_domain_irq(sp_intc.domain, hwirq, regs);
		ret = 0;
	}

	return ret;
}

/* 8388: level-triggered hwirq# may come slower than IRQ */
#define SPURIOUS_RETRY  30

static void __exception_irq_entry sp_intc_handle_irq(struct pt_regs *regs)
{
	int err;
	int first_int = 1;
	int retry = 0;

	dprn("%s\n", __func__);
	do {
		err = sp_intc_handle_one_round(regs);

		if (!err)
			first_int = 0;

		if (first_int && err) { /* spurious irq */
			dprn("+");
			if (retry++ < SPURIOUS_RETRY)
				continue;
		}
	} while (!err);
}

static int sp_intc_irq_domain_map(struct irq_domain *domain, unsigned int irq,
                                  irq_hw_number_t hwirq)
{
	dprn("%s: irq=%d hwirq=%lu\n", __func__, irq, hwirq);

	irq_set_chip_and_handler(irq, &sp_intc_chip, handle_level_irq);
	irq_set_chip_data(irq, &sp_intc_chip);
	irq_set_noprobe(irq);

	return 0;
}

static void __init sp_intc_chip_init(int hwirq_start, int hwirq_end,
				     void __iomem *base0, void __iomem *base1)
{
	int i;

	sp_intc.g0 = base0;
	sp_intc.g1 = base1;
	sp_intc.hwirq_start = hwirq_start;
	sp_intc.hwirq_end = hwirq_end;

	for (i = 0; i < 7; i++) {
		/* all mask */
		writel_relaxed(0, &sp_intc.g0->intr_mask[i]);
		/* all edge */
		writel_relaxed(~0, &sp_intc.g0->intr_type[i]);
		/* all high-active */
		writel_relaxed(0, &sp_intc.g0->intr_polarity[i]);
		/* all irq */
		writel_relaxed(~0, &sp_intc.g0->priority[i]);
		/* all clear */
		writel_relaxed(~0, &sp_intc.g1->intr_clr[i]);
	}
}

static struct irq_domain_ops sp_intc_dm_ops = {
	.xlate = irq_domain_xlate_twocell,
	.map = sp_intc_irq_domain_map,
};

int __init sp_intc_init(int hwirq_start, int irqs, void __iomem *base0, void __iomem *base1)
{
	dprn("%s\n", __func__);

	sp_intc_chip_init(hwirq_start, hwirq_start + irqs, base0, base1);

	sp_intc.domain = irq_domain_add_legacy(NULL, irqs, hwirq_start,
			sp_intc.hwirq_start, &sp_intc_dm_ops, &sp_intc);
	if (!sp_intc.domain)
		panic("%s: unable to create legacy domain\n", __func__);

	set_handle_irq(sp_intc_handle_irq);

	return 0;
}

#ifdef CONFIG_OF
int __init sp_intc_init_dt(struct device_node *node, struct device_node *parent)
{
	void __iomem *base0, *base1;
	int irq;

	dprn("%s\n", __func__);

	base0 = of_iomap(node, 0);
	if (!base0)
		panic("unable to map sp-intc base 0\n");

	base1 = of_iomap(node, 1);
	if (!base1)
		panic("unable to map sp-intc base 1\n");

	sp_intc_chip_init(SP_INTC_HWIRQ_MIN, SP_INTC_HWIRQ_MAX, base0, base1);

	sp_intc.domain = irq_domain_add_linear(node, sp_intc.hwirq_end - sp_intc.hwirq_start,
			&sp_intc_dm_ops, &sp_intc);
	if (!sp_intc.domain)
		panic("%s: unable to create linear domain\n", __func__);

        if (parent) {
		irq = irq_of_parse_and_map(node, 0);
		if (irq) {
			irq_set_chained_handler_and_data(irq,
				sp_intc_handle_irq_cascaded, &sp_intc);
		} else {
			panic("%s: missed irq in DT\n", __func__);
		}
	} else {
		set_handle_irq(sp_intc_handle_irq);
	}

        return 0;
}
IRQCHIP_DECLARE(sp_intc, "sunplus,sp-intc", sp_intc_init_dt);
#endif
