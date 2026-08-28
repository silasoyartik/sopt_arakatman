#!/usr/bin/env python3
"""Build the combined EGL presentation from its Markdown source files."""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import sys
import tempfile


GENERATED_NOTICE = """<!--
AUTO-GENERATED FILE. DO NOT EDIT DIRECTLY.
Run: python scripts/build_presentation.py
Source order: docs/presentation-order.txt
-->"""


class BuildError(Exception):
    """An expected, user-actionable build error."""


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Build docs/eglFunctions.md from ordered Markdown sources."
    )
    parser.add_argument(
        "--order",
        type=Path,
        default=Path("docs/presentation-order.txt"),
        help="order file; entries are relative to this file (default: %(default)s)",
    )
    parser.add_argument(
        "--preamble",
        type=Path,
        default=Path("docs/presentation-preamble.md"),
        help="presentation introduction (default: %(default)s)",
    )
    parser.add_argument(
        "--footer",
        type=Path,
        default=Path("docs/presentation-footer.md"),
        help="presentation ending (default: %(default)s)",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("docs/eglFunctions.md"),
        help="generated Markdown file (default: %(default)s)",
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="fail if the output is missing or out of date; do not write it",
    )
    return parser.parse_args()


def read_markdown(path: Path, description: str) -> str:
    try:
        if not path.is_file():
            raise BuildError(f"{description} is not a file: {path}")
        text = path.read_text(encoding="utf-8")
    except OSError as error:
        raise BuildError(f"cannot read {description} {path}: {error}") from error

    # A fixed newline style makes the generated artifact identical on every OS.
    normalized = "\n".join(text.splitlines()).rstrip("\n")
    if not normalized.strip():
        raise BuildError(f"{description} is empty: {path}")
    return normalized


def read_order(order_path: Path, output_path: Path) -> list[Path]:
    try:
        lines = order_path.read_text(encoding="utf-8").splitlines()
    except OSError as error:
        raise BuildError(f"cannot read order file {order_path}: {error}") from error

    sources: list[Path] = []
    seen: set[Path] = set()
    order_directory = order_path.parent
    resolved_output = output_path.resolve()

    for line_number, raw_line in enumerate(lines, start=1):
        entry = raw_line.strip()
        if not entry or entry.startswith("#"):
            continue

        source = order_directory / entry
        try:
            resolved_source = source.resolve(strict=True)
        except OSError as error:
            raise BuildError(
                f"{order_path}:{line_number}: source does not exist: {entry}"
            ) from error

        if not resolved_source.is_file():
            raise BuildError(
                f"{order_path}:{line_number}: source is not a file: {entry}"
            )
        if resolved_source.suffix.lower() != ".md":
            raise BuildError(
                f"{order_path}:{line_number}: source is not Markdown: {entry}"
            )
        if resolved_source == resolved_output:
            raise BuildError(
                f"{order_path}:{line_number}: output cannot also be a source: {entry}"
            )
        if resolved_source in seen:
            raise BuildError(
                f"{order_path}:{line_number}: duplicate source: {entry}"
            )

        seen.add(resolved_source)
        sources.append(resolved_source)

    if not sources:
        raise BuildError(f"order file contains no Markdown sources: {order_path}")
    return sources


def build_document(
    preamble_path: Path, footer_path: Path, source_paths: list[Path]
) -> str:
    preamble = read_markdown(preamble_path, "preamble")
    footer = read_markdown(footer_path, "footer")
    source_documents = [read_markdown(path, "source") for path in source_paths]

    toc = ["## İçindekiler", ""]
    toc.extend(
        f"{index}. `{path.stem}`" for index, path in enumerate(source_paths, start=1)
    )

    introduction = preamble + "\n\n" + "\n".join(toc)
    sections = [introduction, *source_documents, footer]
    return GENERATED_NOTICE + "\n\n" + "\n\n---\n\n".join(sections) + "\n"


def validate_inputs(preamble_path: Path, footer_path: Path, output_path: Path) -> None:
    resolved_output = output_path.resolve()
    for description, input_path in (
        ("preamble", preamble_path),
        ("footer", footer_path),
    ):
        if input_path.resolve() == resolved_output:
            raise BuildError(f"output cannot also be the {description}: {input_path}")


def write_if_changed(output_path: Path, content: str) -> bool:
    try:
        current = output_path.read_text(encoding="utf-8") if output_path.exists() else None
    except OSError as error:
        raise BuildError(f"cannot read output {output_path}: {error}") from error

    if current == content:
        return False

    try:
        output_path.parent.mkdir(parents=True, exist_ok=True)
        with tempfile.NamedTemporaryFile(
            mode="w",
            encoding="utf-8",
            newline="\n",
            dir=output_path.parent,
            prefix=f".{output_path.name}.",
            suffix=".tmp",
            delete=False,
        ) as temporary_file:
            temporary_file.write(content)
            temporary_path = Path(temporary_file.name)
        os.replace(temporary_path, output_path)
    except OSError as error:
        if "temporary_path" in locals():
            temporary_path.unlink(missing_ok=True)
        raise BuildError(f"cannot write output {output_path}: {error}") from error
    return True


def main() -> int:
    args = parse_args()
    try:
        validate_inputs(args.preamble, args.footer, args.output)
        source_paths = read_order(args.order, args.output)
        content = build_document(args.preamble, args.footer, source_paths)

        if args.check:
            try:
                current = args.output.read_text(encoding="utf-8")
            except FileNotFoundError:
                print(f"ERROR: generated file is missing: {args.output}", file=sys.stderr)
                return 1
            except OSError as error:
                raise BuildError(f"cannot read output {args.output}: {error}") from error
            if current != content:
                print(f"ERROR: generated file is out of date: {args.output}", file=sys.stderr)
                return 1
            print(f"Up to date: {args.output}")
            return 0

        changed = write_if_changed(args.output, content)
        action = "Generated" if changed else "Already up to date"
        print(f"{action}: {args.output}")
        return 0
    except (BuildError, UnicodeError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
