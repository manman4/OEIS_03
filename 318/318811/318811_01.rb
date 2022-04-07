def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a[0]=0
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
  ary = [1]
  (1..n).each{|i|
    m = f(i - 1)
    ary << (1..i).inject(0){|s, j| s + j * phi_ary[j] * ary[-j] * m / f(i - j)}
  }
  ary
end

n = 500
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}