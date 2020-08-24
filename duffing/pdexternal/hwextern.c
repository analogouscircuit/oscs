#include <m_pd.h>

static t_class *hwextern_class;


typedef struct _hwextern {
	t_object x_obj;
	t_int i_count;
	t_float step;
	t_int i_down, i_up;
	t_outlet *f_out, *b_out;
} t_hwextern;


void hwextern_bang(t_hwextern *x)
{
	t_float f = x->i_count;
	t_int step = x->step;
	x->i_count += step;
	
	if (x->i_down - x->i_up) {
		if ((step>0) && (x->i_count > x->i_up)) {
			x->i_count = x->i_down;
			outlet_bang(x->b_out);
		} else if (x->i_count < x->i_down) {
			x->i_count = x->i_up;
			outlet_bang(x->b_out);
		}
	}
	outlet_float(x->f_out, f);
}


void hwextern_reset(t_hwextern *x)
{
	x->i_count = x->i_down;
}


void hwextern_set(t_hwextern *x, t_floatarg f)
{
	x->i_count = f;
}


void hwextern_bound(t_hwextern *x, t_floatarg f1, t_floatarg f2)
{
	x->i_down = (f1<f2)?f1:f2;
	x->i_up   = (f1>f2)?f1:f2;
}


void *hwextern_new(t_floatarg f)
{
	t_hwextern *x = (t_hwextern *)pd_new(hwextern_class);
	t_float f1=0, f2=0;
		
	x->step=1;
	switch(argc) {
	default:
	case 3:
		x->step = atom_getfloat(argv+2);
	case 2:
		f2 = atom_getfloat(argv+1);
	case 1:
		f1 = atom_getfloat(argv);
		break;
	}
	if (argc<2) f2=f1;
	x->i_down = (f1<f2) ? f1 : f2;
	x->i_up = (f1>f2) ? f1 : f2;
	x->i_count=x->i_down;

	inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("list"), gensym("bound"));
	floatinlet_new(&x->x_obj, &x->step);

	x->f_out = outlet_new(&x->x_obj, &s_float);
	x->b_out = outlet_new(&x->x_obj, &s_bang);
		
	return (void *)x;
}



void hwextern_setup(void)
{
	hwextern_class = class_new(gensym("hwextern"),
							   (t_newmethod) hwextern_new,
							   0,
							   sizeof(t_hwextern),
							   CLASS_DEFAULT,
							   A_GIMME, 0);

	class_addbang(hwextern_class, hwextern_bang);
	class_addmethod(hwextern_class,
					(t_method) hwextern_reset,
					gensym("reset"),
					0);
	class_addmethod(hwextern_class,
					(t_method) hwextern_set,
					gensym("set"),
					A_DEFFLOAT,
					0);
	class_addmethod(hwextern_class,
					(t_method) hwextern_bound,
					gensym("bound"),
					A_DEFFLOAT,
					A_DEFFLOAT,
					0);
	class_sethelpsymbol(hwextern_class, gensym("help-hwextern"));
}

