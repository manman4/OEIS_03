# m^n-n-1
def A(m, n)
  ary = [0]
  n.times{|i|
    (m ** i - i..m ** (i + 1) - i - 2).each{|j|
      ary << (0..i).inject(0){|s, k| s + (j + k).to_s(m)[-k - 1].to_i * m ** k}
    }
  }
  ary
end

ary = A(7, 6)
(0..10000).each{|i|
  print i
  print ' '
  puts ary[i]
}
