tau :: Double
tau = pi * 2

-- sample rate (samples/s) -> length (seconds) -> frequency (Hz) -> offset/phase (rad) -> output samples
tone :: Double -> Int -> Double -> Double -> [Double]
tone _ 0 _ _ = []
tone sampRate len freq ph = sin (radFreq * ph) : tone sampRate (pred len) freq (ph + tau * radFreq/sampRate)
    where radFreq = tau * freq

-- threshold -> input amplitude -> output above threshold
thresh :: (Ord a, Num a) => a -> a -> a
thresh th a = max th a - th

-- threshold -> ratio (denominator) -> input samples -> output samples
compress :: Double -> Double -> [Double] -> [Double]
compress thr rat = map (\x -> (1 / rat) * thresh thr x + thr)

distort :: Double -> [Double] -> [Double]
distort thr = map (min thr)

-- rate (1 to 20) -> depth (0 to 1) -> ins -> outs
flange :: Double -> Double -> [Double] -> [Double]
flange m g xs = let ys = foldl (\acc x -> acc ++ [x]) [] xs in 
                map (\z -> z + ys !! [ g * sin $ rate * (fromIntegral y) | y <- [0..length ys]]) xs

main = do putStrLn "hello"