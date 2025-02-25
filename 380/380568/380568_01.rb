def A(a, b)
  queue = [a, b]
  result = []
  ans = []

  while queue.max < 10 ** 13
    # キューから最小の値を取り出す
    current = queue.shift

    # 数字が a と b の両方を含む場合だけ結果に追加
    if current.to_s.include?(a.to_s) && current.to_s.include?(b.to_s)
      result << current
      if current % (a * b) == 0
        ans << current
        break
      end
    end

    # 新しい値を生成してキューに追加
    queue << current * 10 + a
    queue << current * 10 + b

    # キューを小さい順に並べ替え
    queue.sort!
  end
  ans
end

ans = []
(2..8).each{|i|
  (i + 1..9).each{|j|
    p [i, j, A(i, j)]
    if A(i, j).size > 0
      ans << [A(i, j)[0], i, j, A(i, j)[0]/(i*j)]
    end
  }
}
p ans

ans = []
(2..8).each{|i|
  (i + 1..9).each{|j|
    if A(i, j).size > 0
      ans << A(i, j)[0]
    end
  }
}
p ans.sort