module DuffingEuler

"""
	input is a frequency
"""
function duffingeuler(input, x₀, b, k, fs)
	dt = 1.0/fs
	x₁ = zeros(Float64, length(input)); x₁[1] = x₀[1]
	x₂ = zeros(Float64, length(input)); x₂[1] = x₀[2]
	t = 0
	for i ∈ 2:length(input)
		ω = 2π*input[i]
		ω2 = ω*ω
		x₁[i] = x₁[i-1] + dt * x₂[i-1]
		x₂[i] = x₂[i-1] + dt * ( ω2*( b[i-1]*cos(ω*t) - x₁[i-1]^3 ) - ω*k[i-1]*x₂[i-1] )  
		t += dt
	end
	x₂
end

f1(x₁, x₂, b, k, ω, t) = x₂
f2(x₁, x₂, b, k, ω, t) = ω*( ω*(b*cos(ω*t) - x₁*x₁*x₁) - k*x₂ )

function duffingcosrk(input, x₀, b, k, fs)
	h = 1.0/fs
	half_h = 0.5*h
	sixth_h = h/6.0
	x₁ = zeros(Float64,length(input)); x₁[1] = x₀[1]
	x₂ = zeros(Float64,length(input)); x₂[1] = x₀[2]
	t = 0
	for i ∈ 1:length(input)-1
		ω = 2π*input[i]

		K1 = f1(x₁[i], x₂[i], b[i], k[i], ω, t)
		L1 = f2(x₁[i], x₂[i], b[i], k[i], ω, t)

		K2 = f1(x₁[i]+half_h*K1, x₂[i]+half_h*L1, b[i], k[i], ω, t)
		L2 = f2(x₁[i]+half_h*K1, x₂[i]+half_h*L1, b[i], k[i], ω, t)

		K3 = f1(x₁[i]+half_h*K2, x₂[i]+half_h*L2, b[i], k[i], ω, t)
		L3 = f2(x₁[i]+half_h*K2, x₂[i]+half_h*L2, b[i], k[i], ω, t)

		K4 = f1(x₁[i]+h*K3, x₂[i]+h*L3, b[i], k[i], ω, t)
		L4 = f2(x₁[i]+h*K3, x₂[i]+h*L3, b[i], k[i], ω, t)

		x₁[i+1] = x₁[i] + sixth_h * (K1 + 2*K2 + 2*K3 + K4) 
		x₂[i+1] = x₂[i] + sixth_h * (L1 + 2*L2 + 2*L3 + L4)

		t += h
	end
	x₁
end


function duffing(input, x₀, b, k, fs)
	h = 1.0/fs
	half_h = 0.5*h
	sixth_h = h/6.0
	x₁ = zeros(Float64,length(input)); x₁[1] = x₀[1]
	x₂ = zeros(Float64,length(input)); x₂[1] = x₀[2]
	t = 0
	for i ∈ 1:length(input)-1
		ω = 2π*input[i]

		K1 = f1(x₁[i], x₂[i], b[i], k[i], ω, t)
		L1 = f2(x₁[i], x₂[i], b[i], k[i], ω, t)

		K2 = f1(x₁[i]+half_h*K1, x₂[i]+half_h*L1, b[i], k[i], ω, t)
		L2 = f2(x₁[i]+half_h*K1, x₂[i]+half_h*L1, b[i], k[i], ω, t)

		K3 = f1(x₁[i]+half_h*K2, x₂[i]+half_h*L2, b[i], k[i], ω, t)
		L3 = f2(x₁[i]+half_h*K2, x₂[i]+half_h*L2, b[i], k[i], ω, t)

		K4 = f1(x₁[i]+h*K3, x₂[i]+h*L3, b[i], k[i], ω, t)
		L4 = f2(x₁[i]+h*K3, x₂[i]+h*L3, b[i], k[i], ω, t)

		x₁[i+1] = x₁[i] + sixth_h * (K1 + 2*K2 + 2*K3 + K4) 
		x₂[i+1] = x₂[i] + sixth_h * (L1 + 2*L2 + 2*L3 + L4)

		t += h
	end
	x₁
end

end
