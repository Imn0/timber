// @ts-check
// `@type` JSDoc annotations allow editor autocompletion and type checking
// (when paired with `@ts-check`).
// There are various equivalent ways to declare your Docusaurus config.
// See: https://docusaurus.io/docs/api/docusaurus-config

import { themes as prismThemes } from "prism-react-renderer";

// This runs in Node.js - Don't use client-side code here (browser APIs, JSX...)

/** @type {import('@docusaurus/types').Config} */
const config = {
    title: "timber",
    tagline: "A C logging library",
    url: "https://www.imno.ovh",
    baseUrl: "/",
    onBrokenLinks: "throw",
    favicon: "img/favicon.ico",
    organizationName: "imno",
    projectName: "timber",
    markdown: {
        mermaid: true,
        format: "mdx",
    },
    themes: ["@docusaurus/theme-mermaid"],
    presets: [
        [
            "classic",
            {
                docs: {
                    sidebarPath: require.resolve("./sidebars.js"),
                    routeBasePath: "/",
                    editUrl:
                        "https://github.com/Imn0/timber/tree/master/docs/",
                },
                blog: false,
                theme: {
                    customCss: require.resolve("./src/css/custom.css"),
                },
            },
        ],
    ],

    themeConfig: {
        prism: {
            theme: prismThemes.gruvboxMaterialLight,
            darkTheme: prismThemes.gruvboxMaterialDark,
        },
        navbar: {
            title: "Timber",
            logo: {
                alt: "Timber Logo",
                src: "img/logo.svg",
            },
            items: [
                {
                    href: "https://github.com/Imn0/timber",
                    label: "GitHub",
                    position: "right",
                },
            ],
        },
    },
};

module.exports = config;
