module DuffingTests

using DifferentialEquations
using Plots
using WAV
using DSP


################################################################################
# Helper Functions
################################################################################
function zerocrossingclip(vals)
	n = length(vals)
	idx1 = 1
	idx2 = n
	upward = false
	for i ∈ 1:n-1
		if vals[i] <= 0.0 && vals[i+1] >= 0.0 
			idx1 = i
			upward = true
			break
		elseif vals[i] >= 0.0 && vals[i+1] <= 0.0
			idx1 = i
			upward = false
			break
		end
	end
	if !(upward)
		for i ∈ n:-1:2
			if vals[i] <= 0.0 && vals[i-1] >= 0.0 
				idx2 = i
				break
			end
		end
	else
		for i ∈ n:-1:2
			if vals[i] >= 0.0 && vals[i-1] <= 0.0
				idx2 = i
				break
			end
		end
	end
	return vals[idx1:idx2]
end


"""
	makepdsample(sig, n)

Takes an input sample, cuts at zero-crossings and resamples so that the total
length has the form 2^k + 3 for some integer k.  Results are usable with
tabosc4~ in Pure Data.
"""
function makepdsample(sig, n=0)
	sig = zerocrossingclip(sig)
	if n == 0
		n = 2
		while n < length(sig)
			n *= 2
		end
		n += 3
	end
	DSP.Filters.resample(sig, (n+1)/length(sig))[1:n]
end

################################################################################
# ODE Problem
################################################################################

freq = 500.0
ω = 2π*freq
k = 0.05
B = 7.5
f = x -> B*cos(ω*x)
p = [k,f]

function duffing!(du,u,p,t)
	du[1] = u[2]
	du[2] = (ω^2) * (p[2](t) - u[1]*u[1]*u[1]) - ω*p[1]*u[2] 
end

u0 = [0.0,0.0]
t0 = 0.0; t1 = 0.01 
tspan = (t0,t1)

prob = ODEProblem(duffing!, u0, tspan, p)

sol = solve(prob)

ts = range(t0, t1, length=2^14+3)
vals = [sol(t)[1] for t ∈ ts]
vals = makepdsample(vals)
wavwrite(0.8*vals./maximum(vals), "duffing.wav", Fs=48000)
p = plot(sol)
plot!(p, ts, x -> f(x))


end
