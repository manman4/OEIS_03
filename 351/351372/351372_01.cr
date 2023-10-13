def a(n)
  ary = Array(Array(Int32)).new
  (1..n).each{|k|
    (1..k).each{|j|
      (1..j).each{|i|
        break if i * j > k
        if (i + j) ** 2 + (j + k) ** 2 + (k + i) ** 2 == 12 * i * j * k
          ary << [i, j, k]
          p [i, j, k]
        end
      }
    }
  }
  ary
end

p a(5000).flatten

