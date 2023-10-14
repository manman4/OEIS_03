def A(n)
  ary = []
  (1..n).each{|i|
    (i + 1..n).each{|j|
      if i.gcd(j) == 1 && (i - j) % 3 > 0
        ary << 2 * j * j + 3 * i * j + i * i
      end
    }
  }
  ary
end

p A(30).uniq.sort[0..100]
