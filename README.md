# WER-in-cpp

- Based on [WER-in-python](https://github.com/zszyellow/WER-in-python) implementation of computing word-error-rate (WER) using edit distance dynamic programming. I port the algorithm into C++ with new features:
    + Support multi-line reference and hypothesis
    + Improve the runtime and feasibility when aligning long sentences

#### Usage

```bash
./analyze_wer [Reference] [Hypothesis]
```

#### Examples
- Reference `samples/ref`
- Hypothesis `samples/hyp`
- Result: `./analyze_wer samples/ref samples/hyp`

<img src="result.jpg" width="900"/>
