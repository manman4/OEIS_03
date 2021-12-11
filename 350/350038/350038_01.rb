def A(n)
  ary = []
  (1..n).each{|i|
    (i + 1..n).each{|j|
      if i.gcd(j) == 1 && (i - j) % 3 > 0
        x, y, z = j * j, i * j, i * i
        ary << 2 * x + 5 * y + 2 * z
        ary << 3 * x + 3 * y
      end
    }
  }
  ary
end

p A(20).uniq.sort[0..100]
