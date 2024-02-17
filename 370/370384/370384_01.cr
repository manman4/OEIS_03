def a(n)
  return 1 if n == 0
  cnt = 0
  s_ary = Array(Array(Int32)).new
  (1..n - 5).each{|i|
    s_ary << [i, i + 1, i + 2, i + 3, i + 4, i + 5]
  }
  (1..n).to_a.each_permutation{|h|
    flag = true
    (0..n - 6).each{|i|
      j = h[i..i + 5]
      if s_ary.includes?(j)
        flag = false
        break
      end
    }
    if flag
      cnt += 1
    end
  }
  cnt
end

p (0..12).map{|i| a(i)}

