require 'prime'

def A(n)
  return 0 if n < 2
  p_ary = n.prime_division.to_a
  if p_ary.all?{|p, e| p % 10 == 1 or p % 10 == 9}
    return p_ary.inject(1){|s, (p, e)| s * (e + 1)} / 2
  end
  0
end

a = Array.new(1001){-1}
(0..10 ** 6).each{|i|
  j = A(i)
  a[j] = i if a[j] == -1
}
p a