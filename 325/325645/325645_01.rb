def A(m, n)
  ary = [0]
  n.times{|i|
    (m ** i - i..m ** (i + 1) - i - 2).each{|j|
      ary << (0..i).inject(0){|s, k| s + (j + k).to_s(m)[-1 - k].to_i * m ** k}
    }
  }
  ary
end

p A(5, 3)
