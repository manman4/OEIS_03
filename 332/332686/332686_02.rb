def phi(n)
  phi_ary = [0]
  (1..n).each{|i| phi_ary << i}
  (2..n).each{|p|
    if phi_ary[p] == p
      p.step(n, p){|d| phi_ary[d] -= phi_ary[d] / p}
    end
  }
  phi_ary
end

def A(n)
  phi_ary = phi(n)
  ary = [0]
  (1..n).each{|i|
    ary << (1..i).inject(0){|s, j| s + phi_ary[j / (j.gcd(i))]}
  }
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
  