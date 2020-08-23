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


end
