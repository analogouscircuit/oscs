#include <m_pd.h>

static t_class *hwextern_class;


typedef struct _hwextern {
	t_object x_obj;
	t_int i_count;
} t_hwextern;


void hwextern_bang(t_hwextern *x)
{
	t_float f = x->i_count;
	x->i_count++;
	outlet_float(x->x_obj.ob_outlet, f);
}


void *hwextern_new(t_floatarg f)
{
	t_hwextern *x = (t_hwextern *)pd_new(hwextern_class);
	x->i_count = f;
	outlet_new(&x->x_obj, &s_float);
	post("Made a new extern!");

	return (void *)x;
}


void hwextern_setup(void)
{
	hwextern_class = class_new(gensym("hwextern"),
					 			 (t_newmethod) hwextern_new,
								 0,
								 sizeof(t_hwextern),
								 CLASS_DEFAULT,
								 A_DEFFLOAT, 0);

	class_addbang(hwextern_class, hwextern_bang);
}

