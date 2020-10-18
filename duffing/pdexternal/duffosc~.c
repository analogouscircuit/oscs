#include <m_pd.h>
#include <math.h>

#define DD_PI 6.2831855

static t_class *duffosc_tilde_class;


typedef struct _duffosc_tilde {
	t_object x_obj;

	t_sample f_freq;
	t_sample f;

	t_sample f_x1, f_x2; // state of oscillator
	t_sample k, B; 		 // oscilattor parameters
	t_sample t;
	t_sample h, h2, h6;
} t_duffosc_tilde;


static inline t_sample f1(t_duffosc_tilde *x, t_sample x1, t_sample x2)
{
	return x2;		
}

static inline t_sample f2(t_duffosc_tilde *x, t_sample x1, t_sample x2, t_sample f, t_sample k, t_sample B)
{
	return DD_PI*f*( DD_PI*f*(B*cos(DD_PI*f*x->t) - x1*x1*x1) - k*x2);  
}

static t_int *duffosc_tilde_perform(t_int *w)
{
	t_duffosc_tilde *x = (t_duffosc_tilde *)(w[1]);
	t_sample *input = (t_sample *)(w[2]);
	t_sample *k = (t_sample *)(w[3]);
	t_sample *B = (t_sample *)(w[4]);
	t_sample *out 	 = (t_sample *)(w[5]);
	int n 			 = (int)(w[6]);
	t_sample K1, K2, K3, K4, L1, L2, L3, L4;
	t_sample x1, x2;

	x1 = x->f_x1;
	x2 = x->f_x2;
	while (n--) {
		K1 = f1(x, x1, x2);
		L1 = f2(x, x1, x2, *input, *k, *B);
		K2 = f1(x, x1+x->h2*K1, x2+x->h2*L1);
		L2 = f2(x, x1+x->h2*K1, x2+x->h2*L1, *input, *k, *B);
		K3 = f1(x, x1+x->h2*K2, x2+x->h2*L2);
		L3 = f2(x, x1+x->h2*K2, x2+x->h2*L2, *input, *k, *B);
		K4 = f1(x, x1+x->h*K3, x2+x->h*L3);
		L4 = f2(x, x1+x->h*K3, x2+x->h*L3, *input, *k, *B);
		x1 = x1 + x->h6*(K1 + 2*K2 + 2*K3 + K4);
		x2 = x2 + x->h6*(L1 + 2*L2 + 2*L4 + L4);
		*out++ = x1;
		input++; k++; B++;
		x->t += x->h;
	}
	x->f_x1 = x1;
	x->f_x2 = x2;
	return (w+7);
}

static void duffosc_tilde_reset(t_duffosc_tilde *x)
{
	x->f_x1 = x->f_x2 = x->t = 0.0;
}


void duffosc_tilde_dsp(t_duffosc_tilde *x, t_signal **sp)
{
	dsp_add(duffosc_tilde_perform, 6, x,
			sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[0]->s_n);
}


void *duffosc_tilde_new(t_symbol *s, short argc, t_atom *argv)
{
	t_duffosc_tilde *x = (t_duffosc_tilde *)pd_new(duffosc_tilde_class);
	t_sample f0 = 220.0, k0 = 0.05, B0 = 7.5;

	inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("signal"), gensym("signal"));
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("signal"), gensym("signal"));
	outlet_new(&x->x_obj, &s_signal);

	if (argc >= 3) B0 = atom_getfloatarg(2, argc, argv);
	if (argc >= 2) k0 = atom_getfloatarg(1, argc, argv);
	if (argc >= 1) f0 = atom_getfloatarg(0, argc, argv);

	x->f_x1 = x->f_x2 = 0.0;
	x->f_freq = f0;
	x->k = k0; 	  // make this inlet-determined
	x->B = B0; 	  // make this inlet-determined
	x->t = 0.0;
	x->h = 1.0/sys_getsr(); // get samplerate at runtime 
	x->h2 = x->h/2.0; // for RK4
	x->h6 = x->h/6.0; // for RK4

	return (void *)x;
}

void duffosc_tilde_setup(void)
{
	duffosc_tilde_class = class_new(gensym("duffosc~"),
								    (t_newmethod) duffosc_tilde_new,
								    0,
								    sizeof(t_duffosc_tilde),
								    CLASS_DEFAULT,
								    A_DEFFLOAT,
									0);
	CLASS_MAINSIGNALIN(duffosc_tilde_class, t_duffosc_tilde, f);
	class_addmethod(duffosc_tilde_class, (t_method)duffosc_tilde_dsp, gensym("dsp"), 0);
	class_addmethod(duffosc_tilde_class, (t_method)duffosc_tilde_reset, gensym("reset"), 0);
	post("duffosc~, a forced Duffing oscillator from David Dahlbom's pile of crap");
}

