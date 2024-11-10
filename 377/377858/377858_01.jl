using Nemo
RR = ArbField(1000)
function F(n)
    sum(RR(tanpi(QQBar(k) / (1 + 2 * n))^4) for k in 0:n)
end
a(n) = unique_integer(F(n))[2]
println([a(n) for n in 0:34])
