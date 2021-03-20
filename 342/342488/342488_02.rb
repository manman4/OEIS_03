require 'prime'

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
    s = 0
    (1..i).each{|j|
      s += phi_ary[j] ** (i / j + 1) if i % j == 0
    }
    ary << s
  }
  ary
end

n = 10000
ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  # FORMULA確認
  if i.prime?
    k = 1 + (i - 1) ** 2
    break if j != k
  end
  print i
  print ' '
  puts j
}
