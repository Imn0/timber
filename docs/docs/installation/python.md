---
sidebar_position: 2
---
# Python

## Package manager

Python bindings can be installed via a package manager directly from github

````mdx-code-block
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>

<TabItem value="pip" label="pip">
```bash
pip install git+https://github.com/Imn0/timber-py.git
```

</TabItem>

<TabItem value="uv" label="uv">
```bash
uv add git+https://github.com/Imn0/timber-py.git
```

</TabItem>

<TabItem value="requirements" label="requirements.txt">
```text
git+https://github.com/Imn0/timber-py.git
```

</TabItem>

<TabItem value="pyproject" label="pyproject.toml">
```toml
dependencies = [
    "timber @ git+https://github.com/Imn0/timber-py.git"
]
```


</TabItem>

</Tabs>
````

## Manual install

To install bindings manually, for example when a C code change is needed or to create custom version

1. Clone bindings

    ```bash
    git clone --recurse-submodules https://github.com/Imn0/timber-py.git
    ```

2. Compile and install

    ```bash
    pip install ./timber-py
    ```
