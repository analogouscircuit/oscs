#include <m_pd.h>
#include <math.h>

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


inline t_sample f1(t_duffosc_tilde *x, t_sample x1, t_sample x2)
{
	return x2;		
}

inline t_sample f2(t_duffosc_tilde *x, t_sample x1, t_sample x2)
{
	return 6.2831855*x->f_freq*( 6.2831855*x->f_freq*(x->B*cos(6.2831855*x->f_freq*x->t) - x1*x1*x1) - x->k*x2);  
}

t_int *duffosc_tilde_perform(t_int *w)
{
	t_duffosc_tilde *x = (t_duffosc_tilde *)(w[1]);
	t_sample *out 	 = (t_sample *)(w[2]);
	int n 			 = (int)(w[3]);
	t_sample K1, K2, K3, K4, L1, L2, L3, L4;
	t_sample x1, x2;

	x1 = x->f_x1;
	x2 = x->f_x2;

	while (n--) {
		K1 = f1(x, x1, x2);
		L1 = f2(x, x1, x2);

		K2 = f1(x, x1+x->h2*K1, x2+x->h2*L1);
		L2 = f2(x, x1+x->h2*K1, x2+x->h2*L1);

		K3 = f1(x, x1+x->h2*K2, x2+x->h2*L2);
		L3 = f2(x, x1+x->h2*K2, x2+x->h2*L2);

		K4 = f1(x, x1+x->h*K3, x2+x->h*L3);
		L4 = f2(x, x1+x->h*K3, x2+x->h*L3);
		
		x1 = x1 + x->h6*(K1 + 2*K2 + 2*K3 + K4);
		x2 = x2 + x->h6*(L1 + 2*L2 + 2*L4 + L4);

		*out++ =x1;

		x->t += x->h;
	}

	// save updated state for next call
	x->f_x1 = x1;
	x->f_x2 = x2;

	return (w+4);
}


void duffosc_tilde_dsp(t_duffosc_tilde *x, t_signal **sp)
{
	dsp_add(duffosc_tilde_perform, 3, x,
			sp[0]->s_vec, sp[0]->s_n);
}


void *duffosc_tilde_new(t_floatarg f)
{
	t_duffosc_tilde *x = (t_duffosc_tilde *)pd_new(duffosc_tilde_class);
	
	// Initial frequency
	x->f_freq = (f == 0) ? 220.0 : f;

	// Initialize state
	x->f_x1 = x->f_x2 = 0.0;
	x->k = 0.05; 	  // make this inlet-determined
	x->B = 7.5; 	  // make this inlet-determined
	x->t = 0.0;
	x->h = 1.0/48000; // get samplerate at runtime 
	x->h2 = x->h/2.0;
	x->h6 = x->h/6.0;

	// Driving oscillator frequency
	floatinlet_new(&x->x_obj, &x->f_freq);

	// Duffing output
	outlet_new(&x->x_obj, &s_signal);
		
	return (void *)x;
}




void duffosc_tilde_setup(void)
{
	duffosc_tilde_class = class_new(gensym("duffosc~"),
								  (t_newmethod) duffosc_tilde_new,
								  0,
								  sizeof(t_duffosc_tilde),
								  CLASS_DEFAULT,
								  A_DEFFLOAT, 0);

	class_addmethod(duffosc_tilde_class, (t_method)duffosc_tilde_dsp, gensym("dsp"), 0);
	/* CLASS_MAINSIGNALIN(duffosc_tilde_class, t_duffosc_tilde, f); */

}

